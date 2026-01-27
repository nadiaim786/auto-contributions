// Learning Objective: This tutorial demonstrates how to create visually interesting patterns
// by drawing simple geometric shapes repeatedly using C++ and the SFML library.
// We will focus on drawing a "starburst" pattern, illustrating basic loop structures
// and coordinate manipulation for visual effect.

#include <SFML/Graphics.hpp> // Include the SFML graphics module for window and drawing operations
#include <vector>            // Include the vector container for storing shapes
#include <cmath>             // Include cmath for mathematical functions like sin and cos

int main() {
    // --- 1. Window Setup ---
    // Create an SFML window object. This is our drawing canvas.
    // Arguments: VideoMode (width, height, bitsPerPixel), Title String
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Starburst Pattern");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS for smoother animation

    // --- 2. Shape Creation and Data Storage ---
    // We'll draw many lines to form our starburst. A vector is a dynamic array
    // that can grow or shrink as needed, perfect for storing multiple shapes.
    std::vector<sf::Vertex> starburstVertices;

    // Define the center of our starburst pattern.
    float centerX = 400.0f; // Half of the window width
    float centerY = 300.0f; // Half of the window height
    sf::Vector2f center(centerX, centerY); // SFML's 2D vector type for coordinates

    // Define the number of "rays" or lines in our starburst.
    int numberOfRays = 36;
    // Define the length of each ray from the center.
    float rayLength = 200.0f;

    // --- 3. Generating the Starburst Pattern ---
    // This is where the algorithm for creating the pattern comes in.
    // We'll use a loop to draw each ray.
    for (int i = 0; i < numberOfRays; ++i) {
        // Calculate the angle for the current ray.
        // We divide the full circle (2 * PI radians) by the number of rays
        // to get the angular separation between each ray.
        float angle = static_cast<float>(i) * (2.0f * M_PI) / static_cast<float>(numberOfRays);

        // Calculate the end point of the ray using trigonometry (cosine for x, sine for y).
        // sf::Vector2f is useful here as it has x and y components.
        sf::Vector2f rayEndPoint;
        rayEndPoint.x = center.x + rayLength * std::cos(angle); // x = center.x + length * cos(angle)
        rayEndPoint.y = center.y + rayLength * std::sin(angle); // y = center.y + length * sin(angle)

        // Create two sf::Vertex objects to define a line.
        // sf::Vertex requires a position and an optional color.
        sf::Vertex startVertex(center, sf::Color::White);   // The starting point of the ray is the center
        sf::Vertex endVertex(rayEndPoint, sf::Color::White); // The ending point is calculated above

        // Add these two vertices to our vector.
        // SFML can draw primitives using a sequence of vertices.
        // If we add pairs of vertices, SFML will draw lines connecting them.
        starburstVertices.push_back(startVertex);
        starburstVertices.push_back(endVertex);
    }

    // --- 4. The Main Game Loop ---
    // This loop runs continuously as long as the window is open.
    while (window.isOpen()) {
        // --- 5. Event Handling ---
        // This section checks for user input and window events.
        sf::Event event;
        while (window.pollEvent(event)) {
            // If the user clicks the close button (X), close the window.
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // You could add more event handling here (e.g., keyboard presses)
        }

        // --- 6. Drawing ---
        // Clear the window with a background color (e.g., black) each frame.
        // This prevents shapes from previous frames from lingering.
        window.clear(sf::Color::Black);

        // Draw our starburst pattern.
        // The draw function takes an SFML drawable object.
        // We can pass a pointer to our vector and specify the primitive type (lines).
        window.draw(starburstVertices.data(), starburstVertices.size(), sf::Lines);

        // Display the contents of the window.
        // Everything drawn in the frame is now shown on the screen.
        window.display();
    }

    // --- 7. End of Program ---
    // The program exits when the window is closed.
    return 0;
}

/*
Example Usage:

1. Save the code: Save this code as a .cpp file (e.g., starburst_pattern.cpp).

2. Compile: You'll need an SFML installation.
   A typical command line for g++ on Linux/macOS might look like:
   g++ starburst_pattern.cpp -o starburst_pattern -lsfml-graphics -lsfml-window -lsfml-system

   For Windows with MinGW, it might be:
   g++ starburst_pattern.cpp -o starburst_pattern -I"path/to/SFML/include" -L"path/to/SFML/lib" -lsfml-graphics -lsfml-window -lsfml-system

   (Replace "path/to/SFML" with your actual SFML installation directory.)

3. Run: Execute the compiled program:
   ./starburst_pattern
   (Or starburst_pattern.exe on Windows)

   A window should appear displaying a starburst pattern centered in the window.
   You can experiment by changing `numberOfRays` and `rayLength` to see how the pattern changes.
*/