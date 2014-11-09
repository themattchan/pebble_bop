#include <pebble.h>

/* Function: createLayer
 * Creates a layer with the bounds of the screen to display an image
 * Param: window - the window of the app
 * Returns: a bitmap layer for displaying images
 */
BitmapLayer* createLayer(GRect bounds) {
	BitmapLayer* layer = bitmap_layer_create(bounds);
	bitmap_layer_set_alignment(layer, GAlignCenter);
	bitmap_layer_set_background_color(layer, GColorClear);
	return layer;
}

/* Function: createImage
 * Creates a bitmap using the resource id in the .json file
 * Param: id - the resource id of the image
 * Returns: a bitmap image
 */
GBitmap* createImage(uint32_t id) {
	return gbitmap_create_with_resource(id);
}

/* Function: displayImage
 * Adds the image onto the bitmap layer to display it
 * Param: layer - the bitmap layer, image - the bitmap being displayed
 * Returns: nothing
 */
void displayImage(BitmapLayer* layer, GBitmap* image) {
	bitmap_layer_set_bitmap(layer, image);
}

/* Function: deleteImage
 * Unloads the image displayed
 * Param: image - the image being removed
 * Returns: nothing
 */
void deleteImage(GBitmap* image) {
	gbitmap_destroy(image);
}

/* Function: deleteLayer
 * Deletes a bitmap layer
 * Param: layer - the bitmap layer being deleted
 * Returns: nothing
 */
void deleteLayer(BitmapLayer* layer) {
	bitmap_layer_destroy(layer);
}
