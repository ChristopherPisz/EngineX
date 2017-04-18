

------------------------------------------------------------------------------------------------------------
-- FilenameFromPath_64bit
--
-- #3ds max has a bug in the 64 bit version where it returns a GUID instead of a filename for some reason
-- This function works around that

fn FilenameFromPath_64bit filePath =
(
	foo=execute ("@\""+(filePath as string)+"\"")
	return filenamefrompath foo
)