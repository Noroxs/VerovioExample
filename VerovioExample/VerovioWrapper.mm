#import "VerovioWrapper.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <Verovio/Verovio-umbrella.h>

@interface VerovioWrapper()
@property (nonatomic, assign) vrv::Toolkit *toolkit;
@end

@implementation VerovioWrapper

- (instancetype)init
{
    if ((self = [super init])) {
        NSBundle *verovioBundle = [NSBundle bundleWithIdentifier:@"com.rism.VerovioFramework"];
        NSString *resourcePath = [verovioBundle URLsForResourcesWithExtension:@"xml"
                                                                 subdirectory:@"data"].firstObject.URLByDeletingLastPathComponent.path;

        self.toolkit = new vrv::Toolkit(false);

        vrv::Resources::SetPath([resourcePath cStringUsingEncoding:NSUTF8StringEncoding]);

        if (!vrv::Resources::InitFonts()) {
            NSLog(@"Can't init fonts");
            delete self.toolkit;
            return nil;
        }

    }
    return self;
}

- (void)dealloc
{
    delete self.toolkit;
}

- (NSString *)renderFirstPageForURL:(NSURL*)url withSize:(CGSize)size
{
    std::string filePath = std::string([url.path cStringUsingEncoding: NSUTF8StringEncoding]);

    // load file into verovio
    self.toolkit->LoadFile(filePath);

    // set size and scale
    [self setPageSize:size scale:50];

    std::string svg = self.toolkit->RenderToSVG(1);
    return [NSString stringWithCString:svg.c_str() encoding:NSUTF8StringEncoding];
}

- (void)setPageSize:(CGSize)size scale:(NSInteger)scale
{
    float scaleFloat = (float) scale;
    CGFloat scaledHeight = size.height * 100.f / scaleFloat;
    CGFloat scaledWidth = size.width * 100.f / scaleFloat;
    CGSize scaledSize = CGSizeMake(scaledWidth, scaledHeight);

    [self setOptionsSize:scaledSize scale:scale];
    self.toolkit->RedoLayout();
}

- (void)setOptionsSize:(CGSize)size scale:(NSInteger)scale
{
    NSString *options = [NSString stringWithFormat:@"{ \"pageHeight\": %d, \"pageWidth\": %d, \"scale\": %ld, \"adjustPageHeight\": true }", (int)size.height, (int)size.width, (long)scale];

    std::string cOptions = std::string([options cStringUsingEncoding: NSUTF8StringEncoding]);

    self.toolkit->SetOptions(cOptions);
}

@end
