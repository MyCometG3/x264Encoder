#import <QTKit/QTKit.h>

int main (int argc, const char * argv[]) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	// -------------------------------------------------------------------------------------------------
	// Parameters
	NSString *thePath		= @"/Volumes/Data/work/_Movies/_5sec.mov";
	NSString *outputPath	= @"/Volumes/Data/work/_Movies/_out.mov";
	
	// -------------------------------------------------------------------------------------------------
	// Commandline hack
	[NSApplication sharedApplication];
	
	ProcessSerialNumber psn = {0, kCurrentProcess};
	TransformProcessType( &psn, kProcessTransformToForegroundApplication );
	SetFrontProcess( &psn );
	
	// -------------------------------------------------------------------------------------------------
	// Open Source Movie
	NSError* theError = nil;
	QTMovie* theMovie = [QTMovie movieWithFile:thePath error:&theError];
	if( !theMovie ) NSLog(@"ERROR: [QTMovie movieWithFile:error:] = %@", theError);
	if( !theMovie ) goto bail;
	
	// -------------------------------------------------------------------------------------------------
	// Preparation for Movie export component
	ComponentInstance ci; 
	ComponentResult   result = noErr; 
	QTAtomContainer	container = nil;
	BOOL useHighResolutionAudio = TRUE;
	Boolean canceled;
	NSData*	theData = nil;
	
	// -------------------------------------------------------------------------------------------------
	// Build Criteria
	ComponentDescription cd = {0}; 
	cd.componentType = MovieExportType;				// 'spit'
	cd.componentSubType = kQTFileTypeMovie;			// 'MooV'
	cd.componentManufacturer = kAppleManufacturer;	// 'appl'
	cd.componentFlags = canMovieExportFiles;		// 1 << 4
	cd.componentFlagsMask = canMovieExportFiles;	// 1 << 4
	
	// -------------------------------------------------------------------------------------------------
	// Open Movie export component
	result = OpenAComponent(FindNextComponent(0, &cd), &ci);  // Suppose only one component meets criteria...
	if( result ) NSLog(@"OpenComponent:%d",result);
	if( result ) goto bail;
	
	// -------------------------------------------------------------------------------------------------
	// Set up StandardCompressionSubTypeAudio
	result = QTSetComponentProperty(
        ci,
        kQTPropertyClass_MovieExporter,
        kQTMovieExporterPropertyID_EnableHighResolutionAudioFeatures,
        sizeof(Boolean),
        &useHighResolutionAudio);
	if( result ) NSLog(@"QTSetComponentProperty:%d",result);
	if( result ) goto bail;
	
	// -------------------------------------------------------------------------------------------------
	// Show Dialog
	result = MovieExportDoUserDialog(ci,[theMovie quickTimeMovie],nil,0,GetMovieDuration([theMovie quickTimeMovie]),&canceled);
	if( result ) NSLog(@"MovieExportDoUserDialog:%d",result);
	if( result || canceled ) { NSLog(@"Cancel");goto bail; }
	
	// -------------------------------------------------------------------------------------------------
	// Get Settings
	result = MovieExportGetSettingsAsAtomContainer(ci, &container);
	if( result ) NSLog(@"MovieExportDoUserDialog:%d",result);
	if( result ) goto bail;
	
	// -------------------------------------------------------------------------------------------------
	// Create NSData from QTAtomContainer
	HLock(container); 
	theData = [NSData dataWithBytes:*container length:GetHandleSize(container)];
	HUnlock(container); 
	
	// -------------------------------------------------------------------------------------------------
	// Build export attribute for QTMovie
	id exportAttr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:YES], QTMovieExport,
		[NSNumber numberWithLong:kQTFileTypeMovie], QTMovieExportType,
		theData, QTMovieExportSettings,
//		[NSNumber numberWithLong:'appl'], QTMovieExportManufacturer,
		nil];
	
	// -------------------------------------------------------------------------------------------------
	// Export using kQTFileTypeMovie type dictionary
	BOOL theBOOL = [theMovie writeToFile:outputPath withAttributes:exportAttr];
	
	if( theBOOL ) {
		NSLog(@"No error");
	} else {
		NSLog(@"ERROR: [QTMovie writeToFile:withAttributes:] failed.");
	}
	
bail:
	[pool release];
    return 0;
}
