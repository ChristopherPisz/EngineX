
-- Data identifier bytes
global MATERIAL_START = 0
global VERTICES_START = 1
global POLYGONSET_END = 2

-- Material type bytes
global STANDARD_MATERIAL    = 0 

-- Shader Types
global MAX_ANISOTROPIC      = 0
global MAX_BLINN            = 1
global MAX_METAL            = 2
global MAX_MULTILAYER       = 3
global MAX_OREN_NAYAR_BLINN = 4 
global MAX_PHONG            = 5
global MAX_STRAUSS          = 6
global MAX_TRANSLUCENT      = 7

-----------------------------------------------------------------------------------------------------------
-- IsGeometry
-- 
-- @return - true if the superclass of the parameter is a Geometry class and is not a TargetObject
--                In other words, make sure the selected object has a mesh

fn IsGeometry selected = 
(
   Superclassof selected == Geometryclass and classof selected != TargetObject 
)

-----------------------------------------------------------------------------------------------------------
-- WriteMaterialData
-- 
-- @param rootMaterial - The root material to be written out to file
-- @param outFile         - The opened file to write to
--
-- @throws error message string, if there was an error

fn WriteMaterialData rootMaterial binFile =
(
	m_materialType
	m_materialShader
	
	m_ambientMapped
	m_ambientColor
	m_ambientMappedFile
	
	m_emissiveMapped
	m_emissiveColor
	m_emissiveMappedFile
	
	m_diffuseMapped
	m_diffuseColor
	m_diffuseMappedFile
	
	m_specularMapped
	m_specularColor
	m_specularMappedFile
	m_specularExponent
	
	-- Standard Material
	if classOf rootMaterial == standardMaterial then
	(
		m_materialType = STANDARD_MATERIAL
		
		-- Shader Type
		if rootMaterial.shaderType == MAX_ANISOTROPIC or
		   rootMaterial.shaderType == MAX_BLINN or   
		   rootMaterial.shaderType == MAX_METAL or
		   rootMaterial.shaderType == MAX_OREN_NAYAR_BLINN or	
		   rootMaterial.shaderType == MAX_PHONG or 
		   rootMaterial.shaderType == MAX_TRANSLUCENT then
		(
			m_materialShader = rootMaterial.shaderType
			
			-- Ambient
			if rootMaterial.ambientMapEnable == true then
			(
				m_ambientMapped = true
				texMap = rootMaterial.ambientMap
				
				if classOf texMap == BitmapTexture then
				(
					m_ambientMappedFile = filenameFromPath texMap.filename
				)
				else
				(
					throw "One or more materials mapped ambient to something other than a bitmap, which is not supported"
				)
			)
			else
			(
				m_ambientMapped = false
				m_ambientColor = rootMaterial.ambient
			)
			
			-- Emissive
			if rootMaterial.selfIllumMapEnable == true then
			(
				m_emissiveMapped = true
				texMap = rootMaterial.selfIllumMap
				
				if classOf texMap == BitmapTexture then
				(
					m_emissiveMappedFile = filenameFromPath texMap.filename
				)
				else
				(
					throw "One or more materials mapped self Illumination to something other than a bitmap, which is not supported"
				)
			)
			else
			(
				m_emissiveMapped = false
				m_emissiveColor = rootMaterial.selfIllumColor
			)
			
			-- Diffuse
			if rootMaterial.diffuseMapEnable == true then
			(
				m_diffuseMapped = true
				texMap = rootMaterial.diffuseMap
				
				if classOf texMap == BitmapTexture then
				(
					m_diffuseMappedFile = filenameFromPath texMap.filename
				)
				else
				(
					throw "One or more materials mapped diffuse to something other than a bitmap, which is not supported"
				)
			)
			else
			(
				m_diffuseMapped = false
				m_diffuseColor = rootMaterial.diffuse
			)
			
			-- Specular
			-- TODO - not sure how to handle specular exponent. Max uses percentages
			if rootMaterial.specularMapEnable == true then
			(
				m_specularMapped = true
				texMap = rootMaterial.specularMap
				
				if classOf texMap == BitmapTexture then
				(
					m_specularMappedFile = filenameFromPath texMap.filename
				)
				else
				(
					throw "One or more materials mapped specular to something other than a bitmap, which is not supported"
				)
			)
			else
			(
				m_specularMapped = false
				m_specularColor = rootMaterial.diffuse
			)
			
			m_specularExponent = 20.0
			
		)
		else
		(
			throw "One or more materials uses an unsupported shader type"
		)
	)
	else
	(
		throw "Unsupported material type used"
	)
	
	-- Write out the data
	writeByte binFile MATERIAL_START #unsigned
	
	writeByte binFile m_materialType #unsigned
	writeByte binFile m_materialShader #unsigned
	
	if m_ambientMapped then
	(
		writeByte binFile 1 #unsigned
		writeString binFile m_ambientMappedFile
	)
	else
	(
		writeByte binFile 0 #unsigned
		writeFloat binFile (m_ambientColor.r / 255.0)
		writeFloat binFile (m_ambientColor.g / 255.0)
		writeFloat binFile (m_ambientColor.b / 255.0)
		writeFloat binFile (m_ambientColor.a / 255.0)
	)
	
	if m_emissiveMapped then
	(
		writeByte binFile 1 #unsigned
		writeString binFile m_emissiveMappedFile
	)
	else
	(
		writeByte binFile 0 #unsigned
		writeFloat binFile (m_emissiveColor.r / 255.0)
		writeFloat binFile (m_emissiveColor.g / 255.0)
		writeFloat binFile (m_emissiveColor.b / 255.0)
		writeFloat binFile (m_emissiveColor.a / 255.0)
	)
	
	if m_diffuseMapped then
	(
		writeByte binFile 1 #unsigned
		writeString binFile m_diffuseMappedFile
	)
	else
	(
		writeByte binFile 0 #unsigned
		writeFloat binFile (m_diffuseColor.r / 255.0)
		writeFloat binFile (m_diffuseColor.g / 255.0)
		writeFloat binFile (m_diffuseColor.b / 255.0)
		writeFloat binFile (m_diffuseColor.a / 255.0)
	)
	
	if m_specularMapped then
	(
		writeByte binFile 1 #unsigned
		writeString binFile m_specularMappedFile
	)
	else
	(
		writeByte binFile 0 #unsigned
		writeFloat binFile (m_specularColor.r / 255.0)
		writeFloat binFile (m_specularColor.g / 255.0)
		writeFloat binFile (m_specularColor.b / 255.0)
		writeFloat binFile (m_specularColor.a / 255.0)
	)
	
	writeFloat binFile (m_specularExponent)
)

------------------------------------------------------------------------------------------------------------
-- Main
--

global maxToDirectXVert  = matrix3 [1, 0, 0] [0, 0, 1] [0, 1, 0] [0, 0, 0]
global maxToDirectXUVW = matrix3 [1, 0, 0] [0, -1, 0] [0, 0, 1] [0, 0, 0]

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
					
				vertices[faceIndex * 3 + 1].m_texCoord[mapChannel] = meshop.getMapVert tmesh mapChannel mapFace.x
				vertices[faceIndex * 3 + 2].m_texCoord[mapChannel] = meshop.getMapVert tmesh mapChannel mapFace.y
				vertices[faceIndex * 3 + 3].m_texCoord[mapChannel] = meshop.getMapVert tmesh mapChannel mapFace.z
			)
		)
	)
	
	-- Convert to DirectX Coordinate system
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

	-- Open an output file to write to
	outName =  GetSaveFileName()
	
	if outName != undefined then
	(
		binFile = fopen outName "wbc"
		format "Created and opened file: %\n" outName 
		
		-- Material info
		WriteMaterialData rootMaterial binFile
		
		--Write out the data
		writeByte binFile VERTICES_START #unsigned
		writeLong binFile vertices.count #unsigned
		writeByte binFile vertices[1].m_texCoord.count #unsigned
		
		for faceIndex = 1 to vertices.count / 3 do
		(
			for vertexIndex = 1 to 3 do
			(
				position = vertices[(faceIndex - 1) * 3 + vertexIndex].m_position
				writeFloat binFile position.x
				writeFloat binFile position.y
				writeFloat binFile position.z
				
				normal = vertices[(faceIndex - 1) * 3 + vertexIndex].m_normal
				writeFloat binFile normal.x
				writeFloat binFile normal.y
				writeFloat binFile normal.z
				
				for uvSetIndex = 1 to vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord.count do
				(
					texCoord = vertices[(faceIndex - 1) * 3 + vertexIndex].m_texCoord[uvSetIndex]
					
					writeFloat binFile texCoord.x
					writeFloat binFile texCoord.y
--					writeFloat binFile texCoord.z
				)
			)
		)
		
		-- End of this polygon set
		writeByte binFile POLYGONSET_END #unsigned
		
		-- close the file
		fflush binFile
		if fClose binFile == false then
		(
			throw "Could not close file!"
		)
		
		-- clean up
		delete tmesh
	)
)
