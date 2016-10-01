var grey = function() {}

grey.process = function(pixels, width, height) {
	return __grey_process(pixels, width, height);
}
/*
* Main processing function
*/
function __grey_process(pixels, width, height) {

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

		resultPixels[i] = grey;
		resultPixels[i + 1] = grey;
		resultPixels[i + 2] = grey;
		resultPixels[i + 3] = pixels[i + 3];

	}
	/*=================================================================================*/
	return resultPixels;
}