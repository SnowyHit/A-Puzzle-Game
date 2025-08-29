# A-Puzzle-Game
A 3d puzzle game where you take images from an API

# Project Requirements
 - Builded on UE5.4.4 
 - No Plugins needed

Key Components
PuzzleBoard – An AActor that defines the grid layout, builds tile visuals, and manages placement, replacement, or swapping of puzzle pieces within slots.

PuzzlePiece – Actor representing an individual tile. It can initialize from data and uses a dynamic material to display a texture downloaded from the API.

PuzzlePlayerController – Handles mouse input, drag‑and‑drop logic, and uses ray‑casting to position pieces on the board plane.

PuzzleDataManager – Actor dedicated to HTTP communication. It retrieves puzzle piece metadata, fills textures once they’re downloaded, and can post scores to a server.

PuzzleGameState – Tracks player statistics such as move count and elapsed puzzle time, exposing events for UI or other gameplay systems to react to moves and deletions.
