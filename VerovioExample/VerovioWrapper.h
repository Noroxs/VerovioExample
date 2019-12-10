#import <Foundation/Foundation.h>

typedef struct CGSize CGSize;

NS_ASSUME_NONNULL_BEGIN

@interface VerovioWrapper: NSObject

- (instancetype)init;

- (NSString *)renderFirstPageForURL:(NSURL*)url withSize:(CGSize)size NS_SWIFT_NAME(renderFirstPage(url:size:));

@end

NS_ASSUME_NONNULL_END
