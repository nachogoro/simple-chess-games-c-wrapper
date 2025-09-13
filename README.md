# Simple Chess Games C Wrapper

A C wrapper around the
[simple-chess-games](https://github.com/nachogoro/simple-chess-games) C++
library, providing a C-compatible API for chess game functionality.

## Features

- **Exception-safe C API**: All C++ exceptions are caught and converted to
  error codes
- **Memory management**: Clear ownership and cleanup functions
- **Comprehensive chess functionality**: Create games, make moves, check game
  state, and more
- **Both static and dynamic libraries**: Choose the linking approach that suits
  your needs

## Building

### Prerequisites

- CMake 3.14 or later
- C++17 compatible compiler

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)
```

This will produce:
- `libsimplechess-c.so` (shared library)
- `libsimplechess-c.a` (static library)
- `include/simplechess/simplechess.h` (C header file)

## API Overview

### Core Types

```c
typedef enum {
    SIMPLECHESS_SUCCESS = 0,
    SIMPLECHESS_ERROR_INVALID_ARGUMENT = 1,
    SIMPLECHESS_ERROR_ILLEGAL_STATE = 2,
    SIMPLECHESS_ERROR_OUT_OF_MEMORY = 3,
    SIMPLECHESS_ERROR_UNKNOWN = 4
} SimplechessResult;

typedef void* SimplechessGameManager;
typedef void* SimplechessGame;
```

### Basic Usage

```c
#include "simplechess/simplechess.h"

int main() {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;

    // Create game manager
    result = simplechess_game_manager_create(&manager);
    if (result != SIMPLECHESS_SUCCESS) {
        // Handle error
        return 1;
    }

    // Create new game
    result = simplechess_create_new_game(manager, &game);
    if (result != SIMPLECHESS_SUCCESS) {
        simplechess_game_manager_destroy(manager);
        return 1;
    }

    // Use the game...

    // Clean up
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);

    return 0;
}
```

### Key Functions

#### Game Management
- `simplechess_game_manager_create()` - Create a game manager
- `simplechess_game_manager_destroy()` - Destroy a game manager
- `simplechess_create_new_game()` - Create a new game from starting position
- `simplechess_create_game_from_fen()` - Create a game from FEN notation
- `simplechess_game_destroy()` - Destroy a game instance

#### Game Operations
- `simplechess_make_move()` - Make a move
- `simplechess_claim_draw()` - Claim a draw
- `simplechess_resign()` - Resign the game

#### Game Queries
- `simplechess_game_get_state()` - Get current game state
- `simplechess_game_get_active_color()` - Get active player
- `simplechess_game_get_available_moves()` - Get all available moves
- `simplechess_game_get_moves_for_piece()` - Get moves for a specific piece

#### Utilities
- `simplechess_square_from_string()` - Parse square from algebraic notation
- `simplechess_square_to_string()` - Convert square to string
- `simplechess_piece_move_regular()` - Create regular move
- `simplechess_piece_move_promotion()` - Create pawn promotion move

## Testing

A comprehensive test suite is included that covers both success and failure
scenarios:

```bash
cd tests
make test_static        # Run tests with static linking
make test               # Run tests with dynamic linking
```

The test suite includes:
- **Success Cases**: Game creation, moves, state queries, utility functions
- **Error Cases**: Invalid arguments, illegal states, boundary conditions
- **Edge Cases**: Buffer overflows, array bounds, null pointers

## Error Handling

All functions return a `SimplechessResult`. Always check the return value:

```c
SimplechessResult result = simplechess_make_move(manager, game, &move, false, &new_game);
if (result != SIMPLECHESS_SUCCESS) {
    printf("Error: %s\n", simplechess_result_to_string(result));
    // Handle error appropriately
}
```

## Memory Management

- Game managers must be destroyed with `simplechess_game_manager_destroy()`
- Games must be destroyed with `simplechess_game_destroy()`
- The wrapper handles all internal C++ object lifetimes
- Games are immutable - operations return new game instances

## Threading

The wrapper inherits the thread-safety characteristics of the underlying C++
library. Consult the [simple-chess-games
documentation](https://github.com/nachogoro/simple-chess-games) for details.

## Installation

```bash
make install
```

This will install:
- Libraries to `${CMAKE_INSTALL_LIBDIR}`
- Headers to `${CMAKE_INSTALL_INCLUDEDIR}`

## License

This wrapper follows the same license as the underlying
[simple-chess-games](https://github.com/nachogoro/simple-chess-games) library.
