-----------------------------------------------------------------------------------------------------------
-- IsGeometry
-- 
-- @return - true if the superclass of the parameter is a Geometry class and is not a TargetObject
--                In other words, make sure the selected onject has a mesh

fn IsGeometry selected = 
(
   Superclassof selected == Geometryclass and classof selected != TargetObject 
)

-----------------------------------------------------------------------------------------------------------
-- WriteMaterialData
-- 
-- @param rootMaterial - The root material to be written out to file
-- @param outFile         - The opened file to write to

fn WriteMaterialData rootMaterial outFile =
(
	format "Material\n{\n" to:outFile
	format "Name: %\n" rootMaterial.name to:outFile
	
	unsupported = false
	
	-- Standard Material
	if classOf rootMaterial == standardMaterial then
	(
		format "Type: Standard Material\n" to:outFile
		
		-- Blinn
		if rootMaterial.shaderType == 1 then
		(
			format "Shader: Max_Blinn\n" to:outFile
			
			if rootMaterial.mapEnables[1] then
			(
				texMap = rootMaterial.maps[1]
				
				if classOf texMap == BitmapTexture then
				(
					format "Ambient Map: %\n" texMap.filename to:outFile
				)
			)

			if rootMaterial.mapEnables[2] then
			(
				texMap = rootMaterial.maps[2]
				
				if classOf texMap == BitmapTexture then
				(
					format "Diffuse Map: %\n" texMap.filename to:outFile
				)
			)

			if rootMaterial.mapEnables[3] then
			(
				texMap = rootMaterial.maps[3]
				
				if classOf texMap == BitmapTexture then
				(
					format "Specular Map: %\n" texMap.filename to:outFile
				)
			)

			if rootMaterial.mapEnables[4] then
			(
				texMap = rootMaterial.maps[4]
				
				if classOf texMap == BitmapTexture then
				(
					format "Specular Level Map: %\n" texMap.filename to:outFile
				)
			)
		)
	)
	
	if unsupported then
	(
		messageBox "Unsupported Material found, skipping..." "Error"
	)
	
	format "}\n\n" to:outFile
)

------------------------------------------------------------------------------------------------------------
-- Main
--

global maxToDirectXVert  = matrix3 [1, 0, 0] [0, 0, 1] [0, 1, 0] [0, 0, 0]
global maxToDirectXUVW = matrix3 [1, 1, 0] [0, 0, 0] [0, 1, 1] [0, 0, 0]

struct vertex
(
    m_position,
    m_normal,
    m_texCoord = #()
)

-- Check all objects below the mouse pointer and allow objects to be picked that match the filter function
obj = pickobject filter:IsGeometry
	
if isValidNode obj then
(
	-- Take a snapshot of the selected mesh
	tmesh = convertToMesh (snapshot obj)

	-- Get the material
	rootMaterial = obj.material
	
	vertices = #(vertex)
	for vertexIndex = 1 to tmesh.numfaces * 3 do
	(
		vertices[vertexIndex] = vertex()
	)

	-- Get the data for each face
	for faceIndex = 0 to tmesh.numfaces - 1 do
	(
		face = getFace tmesh (faceIndex + 1) -- vtx = a point3 containing 3 vertex indices
		
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
					
				vertices[faceIndex * 3 + 1].m_texCoord[mapChannel] = [(meshop.getMapVert tmesh mapChannel mapFace.x).x, (meshop.getMapVert tmesh mapChannel mapFace.x).y, (meshop.getMapVert tmesh mapChannel mapFace.x).z]
				vertices[faceIndex * 3 + 2].m_texCoord[mapChannel] = [(meshop.getMapVert tmesh mapChannel mapFace.y).x, (meshop.getMapVert tmesh mapChannel mapFace.y).y, (meshop.getMapVert tmesh mapChannel mapFace.y).z]
				vertices[faceIndex * 3 + 3].m_texCoord[mapChannel] = [(meshop.getMapVert tmesh mapChannel mapFace.z).x, (meshop.getMapVert tmesh mapChannel mapFace.z).y, (meshop.getMapVert tmesh mapChannel mapFace.z).z]
			)
		)
	)
	
	-- Convert to DirectX Coordinate system
	for faceIndex = 1 to vertices.count / 3 do
	(
		for vertexIndex = 1 to 3 do
		(
			vertices[(faceIndex - 1) * 3 + vertexIndex].m_position *=  maxToDirectXVert
			vertices[(faceIndex - 1) * 3 + vertexIndex].m_normal *=  maxToDirectXVert
			
			for uvSetIndex = 1 to vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord.count do
			(
				vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord[uvSetIndex] *=  maxToDirectXUVW
			)
		)
		
		temp = vertices[(faceIndex - 1) * 3 + 2]
		vertices[(faceIndex - 1) * 3 + 2] = vertices[(faceIndex - 1) * 3 + 3]
		vertices[(faceIndex - 1) * 3 + 3] = temp
	)
	

	-- Open an output file to write to
	out_name =  GetSaveFileName()
	
	if out_name != undefined then
	(
		out_file = createfile out_name
		
		-- Material info
		WriteMaterialData rootMaterial out_file
		
		-- Write out the data
		for faceIndex = 1 to vertices.count / 3 do
		(
			for vertexIndex = 1 to 3 do
			(
				position = vertices[(faceIndex - 1) * 3 + vertexIndex].m_position
				normal = vertices[(faceIndex - 1) * 3 + vertexIndex].m_normal
				
				format "v % n %" position normal texCoord to:out_file
				
				for uvSetIndex = 1 to vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord.count do
				(
					texCoord = vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord[uvSetIndex]
					format " u[%] %" uvSetIndex texCoord to:out_file
				)
				format "\n" to:out_file
			)
			
			format "\n" to:out_file
		)
		
		-- close the file
		close out_file
		
		-- clean up
		delete tmesh
		
		-- open the file for viewing
		edit out_name
	)
)
