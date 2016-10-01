var gauss = function() {}

gauss.process = function(pixels, width, height, ow, r) {
	return __gauss_process(pixels, width, height, ow);
}
/*
* Main processing function
*/
function __gauss_process(pixels, width, height, ow) {


	var opera = null;
	var fix = 0;

	/* 3x3 guass */
	if(ow == 3) {
		opera = [
			1, 2, 1,
			2, 4, 2, 
			1, 2, 1
		];
		fix = 1/16;
	}
	else if(ow == 5) {
		opera = [
			1, 4, 7, 4, 1,
			4, 16, 26, 16, 4,
			7, 26, 41, 26, 7,
			4, 16, 26, 16, 4,
			1, 4, 7, 4, 1
		];
		fix = 1/273;
	}

	var resultPixels = [];

	resultPixels = convolute.process(pixels, width, height, opera, ow, fix);
	
	/*=================================================================================*/
	return resultPixels;
}