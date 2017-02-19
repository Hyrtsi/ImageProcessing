// ImageProcessing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdio.h>		// NULL (?)
#include <stdlib.h>		// Random
#include <time.h>		// time
#include <math.h>


#define PI 3.14159265


// Note to self:
// Image loading errors vanished when using release mode instead of debug :)


// Forward declarations

std::vector<sf::ConvexShape> fillQuadrilaterals(sf::Image image, unsigned int xWindow, unsigned int yWindow);
std::vector<sf::ConvexShape> fillHexagons(sf::Image image, float hexagonSize);
sf::Image pixelate(sf::Image image, unsigned int xWindow, unsigned int yWindow);
void saveWindow(sf::RenderWindow& window);




int main()
{

	sf::Image image;
	image.loadFromFile("images/cat.png");

	sf::Vector2u windowSize = image.getSize();
	sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "Artsy.");

	srand(time(NULL));		// Init random seed



	// Editing the image

	std::vector<sf::ConvexShape> polygons = fillQuadrilaterals(image, 30,30);

	//std::vector<sf::ConvexShape> hexagons = fillHexagons(image, 6.f);






	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		for (auto& polygon : polygons) window.draw(polygon);

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

void saveWindow(sf::RenderWindow& window) {
	sf::Texture outputTexture;

	outputTexture.create(window.getSize().x, window.getSize().y);
	outputTexture.update(window);

	sf::Image outputImage = outputTexture.copyToImage();
	outputImage.saveToFile("images/image_processed.png");
}