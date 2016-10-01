var binary = function() {}

binary.process = function(pixels, width, height, t) {
	return __binary_process(pixels, width, height, t);
}
/*
* Main processing function
*/
function __binary_process(pixels, width, height, t) {

	var resultPixels = [];
    /*===============================================================================*/
	/* Our teacher said that all things are some "for" loop, here it is
	 * Change the code below, with your knowledge from books.
	*/
	for(var i = 0; i < height * width * 4; i += 4) {

		var r = pixels[i];
		var g = pixels[i + 1];
		var b = pixels[i + 2];

		var grey = (r + g + b) / 3;
		grey = grey >= t ? 255 : 0;

		resultPixels[i] = grey;
		resultPixels[i + 1] = grey;
		resultPixels[i + 2] = grey;
		resultPixels[i + 3] = pixels[i + 3];

	}
	/*=================================================================================*/
	return resultPixels;
}