

include "util.ms"

global MATERIAL_START       = 0

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
-- ExportMaterial
-- 
-- @param rootMaterial - The root material to be written out to file
-- @param outFile         - The opened binary file to write to
--
-- @throws error message string, if there was an error
   
fn ExportMaterial rootMaterial binFile =
(
   format "   Exporting Material: % ...\n" rootMaterial.name
   format "   {\n"

   writeByte binFile MATERIAL_START #unsigned
   format "      Material Start Marker\n"
   format "      Wrote Byte: %\n\n" MATERIAL_START

   -- Standard Material
	if classOf rootMaterial == standardMaterial then
   (
      writeByte binFile STANDARD_MATERIAL #unsigned
      format "      Standard Material Type\n"
      format "      Wrote Byte: %\n\n" STANDARD_MATERIAL
      
      if rootMaterial.shaderType == MAX_BLINN then
      (
         writeByte binFile MAX_BLINN #unsigned
         format "      Blinn Shader Type\n"
         format "      Wrote Byte: %\n\n" MAX_BLINN
      )
      else
      (
         throw "Unsupported shader type used"
      )
      
      -- Ambient
      if rootMaterial.ambientMapEnable == true then
      (
         writeByte binFile 1 #unsigned
         format "      Ambient Color Mapped\n"
         format "      Wrote Byte: %\n\n" 1
         
         texMap = rootMaterial.ambientMap
         if classOf texMap == BitmapTexture then
         (
            writeString binFile (FilenameFromPath_64bit texMap.filename)
            format "      Ambient Color Filename\n"
            format "      Wrote String: %\n\n" (FilenameFromPath_64bit texMap.filename)
         )
         else
         (
            throw "One or more materials mapped ambient to something other than a bitmap, which is not supported"
         )
      )
      else
      (
         writeByte binFile 0 #unsigned
         format "      Ambient Color Not Mapped\n"
         format "      Wrote Byte: %\n\n" 0
         
         writeFloat binFile (rootMaterial.ambient.r / 255.0)
         writeFloat binFile (rootMaterial.ambient.g / 255.0)
         writeFloat binFile (rootMaterial.ambient.b / 255.0)
         writeFloat binFile (rootMaterial.ambient.a / 255.0)
         format "      Ambient Color RGBA\n"
         format "      Wrote 4 Floats: "
         format "% " (rootMaterial.ambient.r / 255.0)
         format "% " (rootMaterial.ambient.g / 255.0)
         format "% " (rootMaterial.ambient.b / 255.0)
         format "% \n\n" (rootMaterial.ambient.a / 255.0)
      )
      
      -- Emissive
      if rootMaterial.selfIllumMapEnable == true then
      (
         writeByte binFile 1 #unsigned
         format "      Emissive Color Mapped\n"
         format "      Wrote Byte: %\n\n" 1
         
         texMap = rootMaterial.selfIllumMap
         if classOf texMap == BitmapTexture then
         (
            writeString binFile (FilenameFromPath_64bit texMap.filename)
            format "      Emissive Color Filename\n"
            format "      Wrote String: %\n\n" (FilenameFromPath_64bit texMap.filename)
         )
         else
         (
            throw "One or more materials mapped emissive color to something other than a bitmap, which is not supported"
         )
      )
      else
      (
         writeByte binFile 0 #unsigned
         format "      Emissive Color Not Mapped\n"
         format "      Wrote Byte: %\n\n" 0
         
         writeFloat binFile (rootMaterial.selfIllumColor.r / 255.0)
         writeFloat binFile (rootMaterial.selfIllumColor.g / 255.0)
         writeFloat binFile (rootMaterial.selfIllumColor.b / 255.0)
         writeFloat binFile (rootMaterial.selfIllumColor.a / 255.0)
         format "      Emissive Color RGBA\n"
         format "      Wrote 4 Floats: "
         format "% " (rootMaterial.selfIllumColor.r / 255.0)
         format "% " (rootMaterial.selfIllumColor.g / 255.0)
         format "% " (rootMaterial.selfIllumColor.b / 255.0)
         format "% \n\n" (rootMaterial.selfIllumColor.a / 255.0)
      )
      
      -- Diffuse
      if rootMaterial.diffuseMapEnable == true then
      (
         writeByte binFile 1 #unsigned
         format "      Diffuse Color Mapped\n"
         format "      Wrote Byte: %\n\n" 1
         
         texMap = rootMaterial.diffuseMap
         if classOf texMap == BitmapTexture then
         (
            writeString binFile (FilenameFromPath_64bit texMap.filename)
            format "      Diffuse Color Filename\n"
            format "      Wrote String: %\n\n" (FilenameFromPath_64bit texMap.filename)
         )
         else
         (
            throw "One or more materials mapped diffuse color to something other than a bitmap, which is not supported"
         )
      )
      else
      (
         writeByte binFile 0 #unsigned
         format "      Diffuse Color Not Mapped\n"
         format "      Wrote Byte: %\n\n" 0
         
         writeFloat binFile (rootMaterial.diffuse.r / 255.0)
         writeFloat binFile (rootMaterial.diffuse.g / 255.0)
         writeFloat binFile (rootMaterial.diffuse.b / 255.0)
         writeFloat binFile (rootMaterial.diffuse.a / 255.0)
         format "      Diffuse Color RGBA\n"
         format "      Wrote 4 Floats: "
         format "% " (rootMaterial.diffuse.r / 255.0)
         format "% " (rootMaterial.diffuse.g / 255.0)
         format "% " (rootMaterial.diffuse.b / 255.0)
         format "% \n\n" (rootMaterial.diffuse.a / 255.0)
      )
      
      -- Specular
      if rootMaterial.specularMapEnable == true then
      (
         writeByte binFile 1 #unsigned
         format "      Specular Color Mapped\n"
         format "      Wrote Byte: %\n\n" 1
         
         texMap = rootMaterial.specularMap
         if classOf texMap == BitmapTexture then
         (
            writeString binFile (FilenameFromPath_64bit texMap.filename)
            format "      Specular Color Filename\n"
            format "      Wrote String: %\n\n" (FilenameFromPath_64bit texMap.filename)
         )
         else
         (
            throw "One or more materials mapped specular color to something other than a bitmap, which is not supported"
         )
      )
      else
      (
         writeByte binFile 0 #unsigned
         format "      Specular Color Not Mapped\n"
         format "      Wrote Byte: %\n\n" 0
         
         writeFloat binFile (rootMaterial.specular.r / 255.0)
         writeFloat binFile (rootMaterial.specular.g / 255.0)
         writeFloat binFile (rootMaterial.specular.b / 255.0)
         writeFloat binFile (rootMaterial.specular.a / 255.0)
         format "      Specular Color RGBA\n"
         format "      Wrote 4 Floats: "
         format "% " (rootMaterial.specular.r / 255.0)
         format "% " (rootMaterial.specular.g / 255.0)
         format "% " (rootMaterial.specular.b / 255.0)
         format "% \n\n" (rootMaterial.specular.a / 255.0)
      )
      
      -- TODO: Support specular level map or one float
      writeFloat binFile (20.0)
      format "      Specular Level\n"
      format "      Wrote Float: 20.0\n\n"
      
   )
   else
   (
      throw "Unsupported material type used"
   )
   
   format "   }\n\n"
)