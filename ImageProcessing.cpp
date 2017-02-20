// ImageProcessing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdio.h>		// NULL (?)
#include <stdlib.h>		// Random
#include <time.h>		// time
#include <math.h>
#include <numeric>		// Inner product

#define PI 3.14159265


// Note to self:
// Image loading errors vanished when using release mode instead of debug :)


// Forward declarations

std::vector<sf::ConvexShape> fillQuadrilaterals(sf::Image image, unsigned int xWindow, unsigned int yWindow);
std::vector<sf::ConvexShape> fillHexagons(sf::Image image, float hexagonSize);
sf::Image voronoi(sf::Image image, unsigned int nPoints);
sf::Image pixelate(sf::Image image, unsigned int xWindow, unsigned int yWindow);
void saveWindow(sf::RenderWindow& window);

std::vector<sf::Image> imageGradient(sf::Image image);


int main()
{

	sf::Image image;
	image.loadFromFile("images/cat.png");

	sf::Vector2u windowSize = image.getSize();
	sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "Artsy.");

	srand(time(NULL));		// Init random seed



	// Editing the image

	//std::vector<sf::ConvexShape> polygons = fillQuadrilaterals(image, 30,30);

	//std::vector<sf::ConvexShape> hexagons = fillHexagons(image, 6.f);




	// Voronoi diagram
	// Commented temporarily away...
	/*

	unsigned int nPoints = 300;
	image = voronoi(image, nPoints);

	// Saving the image
	// Note! It appears not in the image folder but project folder!

	std::cout << "Saving..." << std::endl;


	std::string fileName = "voronoi_";
	std::string pointNumber = std::to_string(nPoints);
	std::string fileType = ".png";
	if (!image.saveToFile(fileName + pointNumber + fileType)) std::cout << "Failed saving image" << std::endl;

	std::cout << "Done!" << std::endl;

	*/





	std::vector<sf::Image> imageGradients = imageGradient(image);
	imageGradients[0].saveToFile("images/results/xRedGradient.png");
	imageGradients[1].saveToFile("images/results/yRedGradient.png");
	imageGradients[2].saveToFile("images/results/xGreenGradient.png");
	imageGradients[3].saveToFile("images/results/yGreenGradient.png");
	imageGradients[4].saveToFile("images/results/xBlueGradient.png");
	imageGradients[5].saveToFile("images/results/yBlueGradient.png");






	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		//for (auto& polygon : polygons) window.draw(polygon);

		//for (auto& hexagon : hexagons) window.draw(hexagon);

		window.display();


		//
		// Saving and drawing the edited image
		// Note: Is it ok to save it every run? Outside this while-loop the saving becomes troublesome....
		//

		// If edited individual pixels (for example pixelate), do this:
		//image.saveToFile("images/image_processed.png");

		// Otherwise:
		saveWindow(window);

	}

	// For debugging, this leaves the console on
	getchar();

	return 0;
}


std::vector<sf::ConvexShape> fillQuadrilaterals(sf::Image image, unsigned int xWindow, unsigned int yWindow) {

	sf::Vector2u imageSize = image.getSize();

	unsigned int xReso = floor(imageSize.x / xWindow);
	unsigned int yReso = floor(imageSize.y / yWindow);



	// First, set up the grid and sample a random point from each cell

	std::vector<std::vector<sf::Vector2f>> samplePoints;

	for (unsigned int m = 0; m < yReso; m++) {

		std::vector<sf::Vector2f> row;

		for (unsigned int n = 0; n < xReso; n++) {

			unsigned int xCorner = n*xWindow + rand() % xWindow;
			unsigned int yCorner = m*yWindow + rand() % yWindow;

			sf::Vector2f point(xCorner, yCorner);
			row.push_back(point);
		}
		samplePoints.push_back(row);
	}




	// Now we have a set of points acting as corners for the quardilaterals
	// Make the tessellation...

	std::vector<sf::ConvexShape> polygons;

	for (unsigned int m = 0; m < yReso - 1; m++) {
		for (unsigned int n = 0; n < xReso - 1; n++) {

			sf::ConvexShape polygon;
			polygon.setPointCount(4);

			polygon.setPoint(0, samplePoints[m][n]);
			polygon.setPoint(1, samplePoints[m + 1][n]);
			polygon.setPoint(2, samplePoints[m + 1][n + 1]);
			polygon.setPoint(3, samplePoints[m][n + 1]);

			polygon.setOutlineColor(image.getPixel(samplePoints[m][n].x, samplePoints[m][n].y));		// Default white
			polygon.setFillColor(image.getPixel(samplePoints[m][n].x + 1, samplePoints[m][n].y + 1));	// Set according to the sampled point color

			polygons.push_back(polygon);
		}
	}

	return(polygons);
}



std::vector<sf::ConvexShape> fillHexagons(sf::Image image, float hexagonSize) {

	sf::Vector2u imageSize = image.getSize();

	// Note: hexagonSize is the "radius" from center to corner (30deg angle)
											
	float hexagonWidth = hexagonSize * cos(30 * PI / 180);					// This is the radius from center to edge (horizonally)
	float hexagonHeight = hexagonSize * sin(30 * PI / 180) + hexagonSize;	// From one center to the next in the y-direction

	unsigned int xTiles = floor(imageSize.x / (2 * hexagonWidth));
	unsigned int yTiles = floor((imageSize.y - hexagonSize) / (hexagonHeight));

	std::vector<sf::ConvexShape> hexagons;
	for (float i = 0; i < xTiles; i++) {
		for (float j = 0; j < yTiles; j++) {

			sf::Vector2f centerPoint(i * 2 * hexagonWidth + hexagonWidth + ((int)j % 2) * hexagonWidth,
				j * hexagonHeight + hexagonHeight);

			sf::ConvexShape hexagon;
			hexagon.setPointCount(6);

			hexagon.setPoint(0, sf::Vector2f(centerPoint.x + hexagonSize * cos(30 * PI / 180), centerPoint.y + hexagonSize * sin(30 * PI / 180)));
			hexagon.setPoint(1, sf::Vector2f(centerPoint.x + hexagonSize * cos(90 * PI / 180), centerPoint.y + hexagonSize * sin(90 * PI / 180)));
			hexagon.setPoint(2, sf::Vector2f(centerPoint.x + hexagonSize * cos(150 * PI / 180), centerPoint.y + hexagonSize * sin(150 * PI / 180)));
			hexagon.setPoint(3, sf::Vector2f(centerPoint.x + hexagonSize * cos(210 * PI / 180), centerPoint.y + hexagonSize * sin(210 * PI / 180)));
			hexagon.setPoint(4, sf::Vector2f(centerPoint.x + hexagonSize * cos(270 * PI / 180), centerPoint.y + hexagonSize * sin(270 * PI / 180)));
			hexagon.setPoint(5, sf::Vector2f(centerPoint.x + hexagonSize * cos(330 * PI / 180), centerPoint.y + hexagonSize * sin(330 * PI / 180)));

			hexagon.setOutlineColor(image.getPixel(centerPoint.x, centerPoint.y));
			hexagon.setFillColor(image.getPixel(centerPoint.x, centerPoint.y));

			hexagons.push_back(hexagon);


		}
	}


	return(hexagons);




}


sf::Image pixelate(sf::Image image, unsigned int xWindow, unsigned int yWindow) {

	sf::Vector2u imageSize = image.getSize();

	unsigned int xReso = floor(imageSize.x / xWindow);
	unsigned int yReso = floor(imageSize.y / yWindow);

	for (unsigned int m = 0; m < yReso; m++) {
		for (unsigned int n = 0; n < xReso; n++) {

			// Sample the middlemost pixel in each patch
			sf::Color color = image.getPixel(n * xWindow + xWindow / 2, m * yWindow + yWindow / 2);


			for (unsigned int x = n*xWindow; x < n*xWindow + xWindow; x++) {
				for (unsigned int y = m*yWindow; y < m*yWindow + yWindow; y++) {
					image.setPixel(x, y, color);
				}
			}
		}
	}


}


sf::Image voronoi(sf::Image image, unsigned int nPoints) {

	sf::Vector2u imageSize = image.getSize();
	std::vector<sf::Vector2f> seedPoints;

	// Generating the seed points.
	// In this case, we randomize them.
	// Nice idea would be to place them around interesting regions according to image gradient perhaps

	for (unsigned int i = 0; i < nPoints; i++) {

		float x = rand() % imageSize.x;
		float y = rand() % imageSize.y;

		seedPoints.push_back(sf::Vector2f(x, y));
	}


	// Voronoi diagram

	
	for (unsigned int x = 0; x < imageSize.x; x++) {
		for (unsigned int y = 0; y < imageSize.y; y++) {


			sf::Vector2f closestPoint;
			float closestDistance = imageSize.x + imageSize.y;		// Init to something "big" wrt to current measure (Lp2)

			// Determine which of the seed points is closest to current (x,y)
			for (auto& seedPoint : seedPoints) {

				float distance = sqrt( pow(seedPoint.x - x, 2) + pow(seedPoint.y - y, 2) );

				if (distance < closestDistance) {
					closestDistance = distance;
					closestPoint = seedPoint;
				}
			}

			// Sample each point (x,y) with the color of the closest seed point
			sf::Color cellColor = image.getPixel(closestPoint.x, closestPoint.y);

			image.setPixel(x, y, cellColor);
		}
	}


	return(image);
	
}


void saveWindow(sf::RenderWindow& window) {

	// This function saves any drawings in the current window

	sf::Texture outputTexture;

	outputTexture.create(window.getSize().x, window.getSize().y);
	outputTexture.update(window);

	sf::Image outputImage = outputTexture.copyToImage();
	outputImage.saveToFile("images/image_processed.png");
}





std::vector<sf::Image> imageGradient(sf::Image image) {

	std::vector<float> convolutionMask{ -1.f, 0.f, 1.f };
	sf::Vector2u imageSize = image.getSize();

	std::vector<sf::Image> imageGradients;

	sf::Image xRed;
	xRed.create(imageSize.x, imageSize.y);
	sf::Image yRed = xRed;
	sf::Image xGreen = xRed;
	sf::Image yGreen = xRed;
	sf::Image xBlue = xRed;
	sf::Image yBlue = xRed;


	// Calculating image gradient

	for (unsigned int y = 1; y < imageSize.y - 1; y++) {

		for (unsigned int x = 1; x < imageSize.x - 1; x++) {

			// We are using the version of the gradient where the gradient is smaller than the original image

			// X-way gradient

			std::vector<float> neighborhood{ (float)image.getPixel(x - 1, y).r, (float)image.getPixel(x, y).r, (float)image.getPixel(x + 1, y).r };
			float convolution = std::inner_product(convolutionMask.begin(), convolutionMask.end(), neighborhood.begin(), 0.f);
			xRed.setPixel(x, y, sf::Color(0, 0, 0, convolution));

			neighborhood = { (float)image.getPixel(x - 1, y).g, (float)image.getPixel(x, y).g, (float)image.getPixel(x + 1, y).g };
			convolution = std::inner_product(convolutionMask.begin(), convolutionMask.end(), neighborhood.begin(), 0.f);
			xGreen.setPixel(x, y, sf::Color(0, 0, 0, convolution));

			neighborhood = { (float)image.getPixel(x - 1, y).b, (float)image.getPixel(x, y).b, (float)image.getPixel(x + 1, y).b };
			convolution = std::inner_product(convolutionMask.begin(), convolutionMask.end(), neighborhood.begin(), 0.f);
			xBlue.setPixel(x, y, sf::Color(0, 0, 0, convolution));


			// Y-way gradient

			neighborhood = { (float)image.getPixel(x, y - 1).r, (float)image.getPixel(x, y).r, (float)image.getPixel(x, y + 1).r };
			convolution = std::inner_product(convolutionMask.begin(), convolutionMask.end(), neighborhood.begin(), 0.f);
			yRed.setPixel(x, y, sf::Color(0, 0, 0, convolution));

			neighborhood = { (float)image.getPixel(x, y - 1).g, (float)image.getPixel(x, y).g, (float)image.getPixel(x, y + 1).g };
			convolution = std::inner_product(convolutionMask.begin(), convolutionMask.end(), neighborhood.begin(), 0.f);
			yGreen.setPixel(x, y, sf::Color(0, 0, 0, convolution));

			neighborhood = { (float)image.getPixel(x, y - 1).b, (float)image.getPixel(x, y).b, (float)image.getPixel(x, y + 1).b };
			convolution = std::inner_product(convolutionMask.begin(), convolutionMask.end(), neighborhood.begin(), 0.f);
			yBlue.setPixel(x, y, sf::Color(0, 0, 0, convolution));


		}

	}

	imageGradients.push_back(xRed);
	imageGradients.push_back(yRed);
	imageGradients.push_back(xGreen);
	imageGradients.push_back(yGreen);
	imageGradients.push_back(xBlue);
	imageGradients.push_back(yBlue);

	return(imageGradients);
}