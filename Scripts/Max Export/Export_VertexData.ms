
global VERTICES_START = 1

struct vertex
(
    m_position,
    m_normal,
    m_texCoord = #()
)

-----------------------------------------------------------------------------------------------------------
-- ConvertToDirectX
--
-- Converts vertex data from 3DS Max coordinate system to DirectX coordinate system
--
-- @param
-- @param
--
-- @throws error message string, if there was an error

fn ConvertToDirectX vertices = 
(
	for faceIndex = 1 to vertices.count / 3 do
	(
		for vertexIndex = 1 to 3 do
		(
			-- position - switch y and z axis
			position = vertices[(faceIndex - 1) * 3 + vertexIndex].m_position
			
			temp = position.y
			position.y = position.z
			position.z = temp
			
			vertices[(faceIndex - 1) * 3 + vertexIndex].m_position = position
			
			-- normals - switch y and z axis
			normal = vertices[(faceIndex - 1) * 3 + vertexIndex].m_normal
			
			temp = normal.y
			normal.y = normal.z
			normal.z = temp
			
			vertices[(faceIndex - 1) * 3 + vertexIndex].m_normal = normal
		)
		
		-- Clockwise draw order of triangles
		temp = vertices[(faceIndex - 1) * 3 + 2]
		vertices[(faceIndex - 1) * 3 + 2] = vertices[(faceIndex - 1) * 3 + 3]
		vertices[(faceIndex - 1) * 3 + 3] = temp
	)
	
	-- uvs - invert the v component
	numMaps = vertices[1].m_texCoord.count
	
	for uvSetIndex = 1 to numMaps do
	(
		-- get the largest v component
		largestV = 0
		
		for i = 1 to vertices.count do
		(
			currentV = vertices[i].m_texCoord[uvSetIndex].y
			
			if currentV > largestV then
			(
				largestV = currentV
			)
		)
		
		-- fix largest V to wrapping boundry
		intPart = largestV as Integer
		difUp = intpart + 1 - largestV
		difDown = largestV - intPart 
		
		if difUP > difDown then
		(
			largestV = intPart + 1
		)
		else
		(
			largestV = intPart
		)
		
		-- Each v component becomes (largest V - V), to flip the v direction
		for i = 1 to vertices.count do
		(
			texCoord = vertices[i].m_texCoord[uvSetIndex]
			texCoord.y = largestV - texCoord.y
			vertices[i].m_texCoord[uvSetIndex] = texCoord
		)
	)
)

-----------------------------------------------------------------------------------------------------------
-- ExportVertexData
-- 
-- @param tmesh    - The mesh containing the vertex data to be written out to file
-- @param outFile  - The opened binary file to write to
--
-- @throws error message string, if there was an error

fn ExportVertexData tmesh binFile =
(
   format "   Exporting Vertex Data...\n"
   format "   {\n"
	
   vertices = #(vertex)
	for vertexIndex = 1 to tmesh.numfaces * 3 do
	(
		vertices[vertexIndex] = vertex()
	)

	-- Get the data for each face
	for faceIndex = 0 to tmesh.numfaces - 1 do
	(
		face = getFace tmesh (faceIndex + 1) -- a point3 containing 3 vertex indices
		
		-- Get the positions
		vertices[faceIndex * 3 + 1].m_position = getVert tmesh face.x
		vertices[faceIndex * 3 + 2].m_position = getVert tmesh face.y
		vertices[faceIndex * 3 + 3].m_position = getVert tmesh face.z

		-- Get the normals
		normal = meshop.getFaceRNormals tmesh (faceIndex + 1)
		vertices[faceIndex * 3 + 1].m_normal = copy normal[1]
		vertices[faceIndex * 3 + 2].m_normal = copy normal[2]
		vertices[faceIndex * 3 + 3].m_normal = copy normal[3]

		-- Get the UVs
		for mapChannel = 1 to meshop.getNumMaps tmesh do
		(
			if meshop.getMapSupport tmesh mapChannel then
			(
				mapFace = meshop.getMapFace tmesh mapChannel (faceIndex + 1)
					
				vertices[faceIndex * 3 + 1].m_texCoord[mapChannel] = meshop.getMapVert tmesh mapChannel mapFace.x
				vertices[faceIndex * 3 + 2].m_texCoord[mapChannel] = meshop.getMapVert tmesh mapChannel mapFace.y
				vertices[faceIndex * 3 + 3].m_texCoord[mapChannel] = meshop.getMapVert tmesh mapChannel mapFace.z
			)
		)
	)
	
   -- Convert the vertex data to DirectX coordinate system
   ConvertToDirectX vertices
   
   --Write out the data
   writeByte binFile VERTICES_START #unsigned
   format "      Vertices Start Marker\n"
   format "      Wrote Byte: %\n\n" VERTICES_START
   
   writeLong binFile vertices.count #unsigned
   format "      Number of Vertices\n"
   format "      Wrote Byte: %\n\n" vertices.count
   
   writeByte binFile vertices[1].m_texCoord.count #unsigned
   
   for faceIndex = 1 to vertices.count / 3 do
   (
      format "      Face %\n" faceIndex
      format "      {\n" 
      
      for vertexIndex = 1 to 3 do
      (
         format "         Vertex %\n" vertexIndex
         format "         {\n"
         
         position = vertices[(faceIndex - 1) * 3 + vertexIndex].m_position
         writeFloat binFile position.x
         writeFloat binFile position.y
         writeFloat binFile position.z
         format "            Position\n"
         format "            Wrote 3 Floats: %, %, %\n\n" position.x position.y position.z  
         
         normal = vertices[(faceIndex - 1) * 3 + vertexIndex].m_normal
         writeFloat binFile normal.x
         writeFloat binFile normal.y
         writeFloat binFile normal.z
         format "            Normal\n"
         format "            Wrote 3 Floats: %, %, %\n\n" normal.x normal.y normal.z
         
         for uvSetIndex = 1 to vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord.count do
         (
            texCoord = vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord[uvSetIndex]
            
            writeFloat binFile texCoord.x
            writeFloat binFile texCoord.y
	         format "            UV\n"
            format "            Wrote 2 Floats: %, %\n" texCoord.x texCoord.y
         )
         
         format "         }\n"
      )
      
      format "      }\n"
   )

   format "   }\n\n"
)