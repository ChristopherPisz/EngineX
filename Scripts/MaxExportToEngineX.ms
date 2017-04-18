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
global rootMaterial           -- material assigned to the mesh
global positions = #()      -- array of all positions  
global normals =#()         -- array of normals, one per face
global UVs =#(#())         -- 2D array, index 1 - map channel, index 2 - corresponding position index
global faces =#()            -- array of all faces, with each face being 3 vertex indices

global maxToDirectXVert  = matrix3 [1, 0, 0] [0, 0, 1] [0, 1, 0] [0, 0, 0]
global maxToDirectXUVW = matrix3 [1, 1, 0] [0, 0, 0] [0, 1, 1] [0, 0, 0]
	
-- Check all objects below the mouse pointer and allow objects to be picked that match the filter function
obj = pickobject filter:IsGeometry
	
if isValidNode obj then
(
	-- Take a snapshot of the selected mesh
	tmesh = convertToMesh (snapshot obj)

	-- Get the material
	rootMaterial = obj.material
	
	--  Get vertices data
	for vertexIndex = 1 to tmesh.numverts do
	(
		position = getVert tmesh vertexIndex
		positions[vertexIndex] = position
	)

	for faceIndex = 1 to tmesh.numFaces do
	(
		normal = getFaceNormal tmesh faceIndex
		
		for vertexIndex = 1 to 3 do
		(
			normals[(faceIndex - 1) * 3 + vertexIndex] = normal
		)
	)
	
	-- Get the UVs
	for mapChannel = 1 to meshop.getNumMaps tmesh do
	(
		if meshop.getMapSupport tmesh mapChannel then
		(
			for mapFace = 1 to meshop.getNumMapFaces tmesh mapChannel do
			(
				face = meshop.getMapFace tmesh mapChannel mapFace
				
				for i = 1 to 3 do
				(
					mapVertIndex = face[i]
					texCoord = meshop.getMapVert tmesh mapChannel mapVertIndex
					UVs[mapChannel][mapVertIndex] = texCoord
				)
			)
		)
	)
	
	-- Get the faces
	for faceIndex = 1 to tmesh.numfaces do
	(
		faceVertexIndices = (getFace tmesh faceIndex)
		faces[faceIndex] = faceVertexIndices
	)
	
	
	
	-- Convert to DirectX coordinate system and draw order
    
	-- v0 v1 v2 -> v0 v2 v3, to draw in clockwise order for DirectX
	--	temp = faceVertexIndices[2]
	--	faceVertexIndices[2] = faceVertexIndices[3]
	--	faceVertexIndices[3] = temp 
	
	
	
	
	-- Open an output file to write to
	out_name =  GetSaveFileName()
	
	if out_name != undefined then
	(
		out_file = createfile out_name
		
		-- Material info
		WriteMaterialData rootMaterial out_file
		
		-- Write out the data
		for faceIndex = 0 to faces.count do
		(
			faceVertexIndices = faces[faceIndex]
			
			for vertexIndex = 1 to 3 do
			(
				position = positions[faceVertexIndices[vertexIndex]]
				normal = normals[faceVertexIndices[vertexIndex]]
				
				format "\nf[%][%] v% n%" faceIndex vertexIndex position normal to:out_file
			
				for uvSetIndex = 1 to UVs.count do
				(
					uvw = UVs[uvSetIndex][vertexIndex]
					format " u[%] %" uvSetIndex uvw to:out_file 
				)
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
