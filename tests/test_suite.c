/**
 * @file test_suite.c
 * @brief Comprehensive test suite for the Simple Chess Games C wrapper
 *
 * This test suite covers both success and failure scenarios for all
 * API functions, including edge cases and error conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "simplechess/simplechess.h"

/* ========================================================================== */
/* Test Framework                                                             */
/* ========================================================================== */

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

#define TEST(name) \
    do { \
        printf("Running test: %s\n", #name); \
        test_count++; \
        if (name()) { \
            printf("  ✓ PASSED\n"); \
            test_passed++; \
        } else { \
            printf("  ✗ FAILED\n"); \
            test_failed++; \
        } \
        printf("\n"); \
    } while(0)

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("    Assertion failed: %s (line %d)\n", #condition, __LINE__); \
            return 0; \
        } \
    } while(0)

#define ASSERT_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("    Expected %d, got %d (line %d)\n", (int)(expected), (int)(actual), __LINE__); \
            return 0; \
        } \
    } while(0)

#define ASSERT_STR_EQ(actual, expected) \
    do { \
        if (strcmp((actual), (expected)) != 0) { \
            printf("    Expected \"%s\", got \"%s\" (line %d)\n", (expected), (actual), __LINE__); \
            return 0; \
        } \
    } while(0)

/* ========================================================================== */
/* Success Case Tests                                                         */
/* ========================================================================== */

/**
 * Test basic game manager creation and destruction
 */
static int test_game_manager_lifecycle(void) {
    SimplechessGameManager manager;
    SimplechessResult result;

    // Test creation
    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(manager != NULL);

    // Test destruction (should not crash)
    simplechess_game_manager_destroy(manager);

    // Test destroying NULL (should not crash)
    simplechess_game_manager_destroy(NULL);

    return 1;
}

/**
 * Test creating a new game from starting position
 */
static int test_create_new_game(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessGameState state;
    SimplechessColor active_color;
    SimplechessResult result;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Create new game
    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(game != NULL);

    // Check game state
    result = simplechess_game_get_state(game, &state);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(state, SIMPLECHESS_GAME_STATE_PLAYING);

    // Check active color
    result = simplechess_game_get_active_color(game, &active_color);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(active_color, SIMPLECHESS_COLOR_WHITE);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test creating a game from FEN notation
 */
static int test_create_game_from_fen(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessGameState state;
    SimplechessColor active_color;
    SimplechessResult result;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Standard starting position FEN
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    result = simplechess_create_game_from_fen(manager, fen, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check game state
    result = simplechess_game_get_state(game, &state);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(state, SIMPLECHESS_GAME_STATE_PLAYING);

    // Check active color
    result = simplechess_game_get_active_color(game, &active_color);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(active_color, SIMPLECHESS_COLOR_WHITE);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test square utility functions
 */
static int test_square_utilities(void) {
    SimplechessSquare square;
    SimplechessResult result;
    char buffer[10];

    // Test creating square from rank and file
    result = simplechess_square_from_rank_and_file(4, 'e', &square);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(square.rank, 4);
    ASSERT_EQ(square.file, 'e');

    // Test creating square from string
    result = simplechess_square_from_string("a1", &square);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(square.rank, 1);
    ASSERT_EQ(square.file, 'a');

    // Test case insensitive parsing
    result = simplechess_square_from_string("H8", &square);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(square.rank, 8);
    ASSERT_EQ(square.file, 'h');

    // Test square to string
    result = simplechess_square_to_string(&square, buffer, sizeof(buffer));
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_STR_EQ(buffer, "h8");

    return 1;
}

/**
 * Test piece move creation
 */
static int test_piece_moves(void) {
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e2 = {2, 'e'};
    SimplechessSquare e4 = {4, 'e'};
    SimplechessSquare e8 = {8, 'e'};
    SimplechessPieceMove move;
    SimplechessResult result;

    // Test regular move
    result = simplechess_piece_move_regular(&pawn, &e2, &e4, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(move.piece.type, SIMPLECHESS_PIECE_TYPE_PAWN);
    ASSERT_EQ(move.piece.color, SIMPLECHESS_COLOR_WHITE);
    ASSERT_EQ(move.src.rank, 2);
    ASSERT_EQ(move.src.file, 'e');
    ASSERT_EQ(move.dst.rank, 4);
    ASSERT_EQ(move.dst.file, 'e');
    ASSERT(!move.is_promotion);

    // Test promotion move
    result = simplechess_piece_move_promotion(&pawn, &e2, &e8, SIMPLECHESS_PIECE_TYPE_QUEEN, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(move.is_promotion);
    ASSERT_EQ(move.promoted_type, SIMPLECHESS_PIECE_TYPE_QUEEN);

    return 1;
}

/**
 * Test making moves and game progression
 */
static int test_make_moves(void) {
    SimplechessGameManager manager;
    SimplechessGame game, new_game;
    SimplechessResult result;
    SimplechessColor active_color;
    size_t moves_count;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check initial moves count
    result = simplechess_game_get_available_moves_count(game, &moves_count);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(moves_count, 20); // Standard opening has 20 moves

    // Make a move: e2-e4
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e2 = {2, 'e'};
    SimplechessSquare e4 = {4, 'e'};
    SimplechessPieceMove move;

    result = simplechess_piece_move_regular(&pawn, &e2, &e4, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_make_move(manager, game, &move, false, &new_game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check that active color changed
    result = simplechess_game_get_active_color(new_game, &active_color);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(active_color, SIMPLECHESS_COLOR_BLACK);

    simplechess_game_destroy(new_game);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test getting moves for specific pieces
 */
static int test_piece_specific_moves(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;
    SimplechessSquare e2 = {2, 'e'};
    size_t moves_count;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check moves for pawn on e2
    result = simplechess_game_get_moves_for_piece_count(game, &e2, &moves_count);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(moves_count, 2); // e3 and e4

    // Get the actual moves
    SimplechessPieceMove moves[2];
    result = simplechess_game_get_moves_for_piece(game, &e2, moves, 2);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test resignation
 */
static int test_resignation(void) {
    SimplechessGameManager manager;
    SimplechessGame game, resigned_game;
    SimplechessResult result;
    SimplechessGameState state;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // White resigns
    result = simplechess_resign(manager, game, SIMPLECHESS_COLOR_WHITE, &resigned_game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check game state
    result = simplechess_game_get_state(resigned_game, &state);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(state, SIMPLECHESS_GAME_STATE_BLACK_WON);

    simplechess_game_destroy(resigned_game);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/* ========================================================================== */
/* Error Case Tests                                                           */
/* ========================================================================== */

/**
 * Test null pointer error handling
 */
static int test_null_pointer_errors(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;

    // Test null pointer for game manager creation
    result = simplechess_game_manager_create(NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Create valid manager for other tests
    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test null pointer for game creation
    result = simplechess_create_new_game(NULL, &game);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_create_new_game(manager, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test null pointer for FEN game creation
    result = simplechess_create_game_from_fen(manager, NULL, &game);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_create_game_from_fen(manager, "valid fen", NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test invalid FEN strings
 */
static int test_invalid_fen(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test various invalid FEN strings
    const char* invalid_fens[] = {
        "",
        "invalid",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP", // incomplete
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1", // invalid active color
        NULL
    };

    for (int i = 0; invalid_fens[i] != NULL; i++) {
        result = simplechess_create_game_from_fen(manager, invalid_fens[i], &game);
        ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);
    }

    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test invalid square creation
 */
static int test_invalid_squares(void) {
    SimplechessSquare square;
    SimplechessResult result;

    // Test null pointer
    result = simplechess_square_from_rank_and_file(1, 'a', NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test invalid rank
    result = simplechess_square_from_rank_and_file(0, 'a', &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_square_from_rank_and_file(9, 'a', &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test invalid file
    result = simplechess_square_from_rank_and_file(1, 'z', &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test invalid string parsing
    result = simplechess_square_from_string(NULL, &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_square_from_string("z9", &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_square_from_string("", &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_square_from_string("a", &square);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    return 1;
}

/**
 * Test invalid moves
 */
static int test_invalid_moves(void) {
    SimplechessGameManager manager;
    SimplechessGame game, new_game;
    SimplechessResult result;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Try to make an illegal move (e.g., king two squares forward)
    SimplechessPiece king = {SIMPLECHESS_PIECE_TYPE_KING, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e1 = {1, 'e'};
    SimplechessSquare e3 = {3, 'e'};
    SimplechessPieceMove move;

    result = simplechess_piece_move_regular(&king, &e1, &e3, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS); // Move creation should succeed

    // But making the move should fail
    result = simplechess_make_move(manager, game, &move, false, &new_game);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_ILLEGAL_STATE);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test operations on finished games
 */
static int test_finished_game_operations(void) {
    SimplechessGameManager manager;
    SimplechessGame game, resigned_game, new_game;
    SimplechessResult result;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Resign the game
    result = simplechess_resign(manager, game, SIMPLECHESS_COLOR_WHITE, &resigned_game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Try to make a move on the finished game
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_BLACK};
    SimplechessSquare e7 = {7, 'e'};
    SimplechessSquare e5 = {5, 'e'};
    SimplechessPieceMove move;

    result = simplechess_piece_move_regular(&pawn, &e7, &e5, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_make_move(manager, resigned_game, &move, false, &new_game);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_ILLEGAL_STATE);

    // Try to resign again
    result = simplechess_resign(manager, resigned_game, SIMPLECHESS_COLOR_BLACK, &new_game);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_ILLEGAL_STATE);

    simplechess_game_destroy(resigned_game);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test buffer overflow protection
 */
static int test_buffer_overflow_protection(void) {
    SimplechessSquare square = {1, 'a'};
    SimplechessResult result;
    char buffer[2]; // Too small

    // Test buffer too small
    result = simplechess_square_to_string(&square, buffer, sizeof(buffer));
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test null buffer
    result = simplechess_square_to_string(&square, NULL, 10);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    return 1;
}

/**
 * Test array size validation
 */
static int test_array_size_validation(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;
    SimplechessPieceMove moves[1]; // Too small array
    size_t moves_count;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Get actual count (should be 20)
    result = simplechess_game_get_available_moves_count(game, &moves_count);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Try to get moves with array too small
    result = simplechess_game_get_available_moves(game, moves, 1);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test draw reason on non-drawn game
 */
static int test_draw_reason_error(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;
    SimplechessDrawReason reason;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Try to get draw reason on non-drawn game
    result = simplechess_game_get_draw_reason(game, &reason);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_ILLEGAL_STATE);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test invalid promotion piece types
 */
static int test_invalid_promotion(void) {
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_WHITE};
    SimplechessPiece king = {SIMPLECHESS_PIECE_TYPE_KING, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e7 = {7, 'e'};
    SimplechessSquare e8 = {8, 'e'};
    SimplechessPieceMove move;
    SimplechessResult result;

    // Test promoting to king (invalid)
    result = simplechess_piece_move_promotion(&pawn, &e7, &e8, SIMPLECHESS_PIECE_TYPE_KING, &move);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test promoting to pawn (invalid)
    result = simplechess_piece_move_promotion(&pawn, &e7, &e8, SIMPLECHESS_PIECE_TYPE_PAWN, &move);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test promoting non-pawn (invalid)
    result = simplechess_piece_move_promotion(&king, &e7, &e8, SIMPLECHESS_PIECE_TYPE_QUEEN, &move);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test valid promotions (should succeed)
    result = simplechess_piece_move_promotion(&pawn, &e7, &e8, SIMPLECHESS_PIECE_TYPE_QUEEN, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_piece_move_promotion(&pawn, &e7, &e8, SIMPLECHESS_PIECE_TYPE_ROOK, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_piece_move_promotion(&pawn, &e7, &e8, SIMPLECHESS_PIECE_TYPE_BISHOP, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_piece_move_promotion(&pawn, &e7, &e8, SIMPLECHESS_PIECE_TYPE_KNIGHT, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    return 1;
}

/* ========================================================================== */
/* Utility Tests                                                              */
/* ========================================================================== */

/**
 * Test result to string conversion
 */
static int test_result_to_string(void) {
    const char* str;

    str = simplechess_result_to_string(SIMPLECHESS_SUCCESS);
    ASSERT_STR_EQ(str, "Success");

    str = simplechess_result_to_string(SIMPLECHESS_ERROR_INVALID_ARGUMENT);
    ASSERT_STR_EQ(str, "Invalid argument");

    str = simplechess_result_to_string(SIMPLECHESS_ERROR_ILLEGAL_STATE);
    ASSERT_STR_EQ(str, "Illegal state");

    str = simplechess_result_to_string(SIMPLECHESS_ERROR_OUT_OF_MEMORY);
    ASSERT_STR_EQ(str, "Out of memory");

    str = simplechess_result_to_string(SIMPLECHESS_ERROR_UNKNOWN);
    ASSERT_STR_EQ(str, "Unknown error");

    // Test invalid result code
    str = simplechess_result_to_string((SimplechessResult)999);
    ASSERT_STR_EQ(str, "Invalid result code");

    return 1;
}

/* ========================================================================== */
/* Extended API Tests                                                         */
/* ========================================================================== */

/**
 * Test game history functionality
 */
static int test_game_history(void) {
    SimplechessGameManager manager;
    SimplechessGame game, game_after_move;
    SimplechessGameStage stage;
    SimplechessPlayedMove played_move;
    SimplechessResult result;
    size_t history_length;
    bool has_move;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check initial history length (should be 1 - initial position)
    result = simplechess_game_get_history_length(game, &history_length);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(history_length, 1);

    // Get initial stage
    result = simplechess_game_get_stage_at(game, 0, &stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Initial stage should have no move
    result = simplechess_stage_get_move(stage, &played_move, &has_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!has_move);

    simplechess_game_stage_destroy(stage);

    // Make a move
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e2 = {2, 'e'};
    SimplechessSquare e4 = {4, 'e'};
    SimplechessPieceMove move;

    result = simplechess_piece_move_regular(&pawn, &e2, &e4, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_make_move(manager, game, &move, false, &game_after_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Check history length after move (should be 2)
    result = simplechess_game_get_history_length(game_after_move, &history_length);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(history_length, 2);

    // Get stage after move
    result = simplechess_game_get_stage_at(game_after_move, 1, &stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // This stage should have a move
    result = simplechess_stage_get_move(stage, &played_move, &has_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(has_move);

    // Test getting current stage
    SimplechessGameStage current_stage;
    result = simplechess_game_get_current_stage(game_after_move, &current_stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    simplechess_played_move_destroy(played_move);
    simplechess_game_stage_destroy(current_stage);
    simplechess_game_stage_destroy(stage);
    simplechess_game_destroy(game_after_move);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test game stage functionality
 */
static int test_game_stage_functionality(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessGameStage stage;
    SimplechessBoard board;
    SimplechessResult result;
    SimplechessColor color;
    uint8_t castling_rights;
    uint16_t halfmoves, fullmoves;
    char fen_buffer[100];

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Get current stage
    result = simplechess_game_get_current_stage(game, &stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test getting board from stage
    result = simplechess_stage_get_board(stage, &board);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test getting active color
    result = simplechess_stage_get_active_color(stage, &color);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(color, SIMPLECHESS_COLOR_WHITE);

    // Test getting castling rights
    result = simplechess_stage_get_castling_rights(stage, &castling_rights);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(castling_rights, SIMPLECHESS_CASTLING_WHITE_KINGSIDE | SIMPLECHESS_CASTLING_WHITE_QUEENSIDE |
                              SIMPLECHESS_CASTLING_BLACK_KINGSIDE | SIMPLECHESS_CASTLING_BLACK_QUEENSIDE);

    // Test getting halfmove clock
    result = simplechess_stage_get_halfmove_clock(stage, &halfmoves);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(halfmoves, 0);

    // Test getting fullmove counter
    result = simplechess_stage_get_fullmove_counter(stage, &fullmoves);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(fullmoves, 1);

    // Test getting FEN
    result = simplechess_stage_get_fen(stage, fen_buffer, sizeof(fen_buffer));
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_STR_EQ(fen_buffer, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    simplechess_board_destroy(board);
    simplechess_game_stage_destroy(stage);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test played move functionality
 */
static int test_played_move_functionality(void) {
    SimplechessGameManager manager;
    SimplechessGame game, game_after_move;
    SimplechessGameStage stage;
    SimplechessPlayedMove played_move;
    SimplechessPieceMove piece_move;
    SimplechessPiece captured_piece;
    SimplechessCheckType check_type;
    SimplechessResult result;
    bool has_move, has_capture, is_draw_offered;
    char notation_buffer[20];

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Make a move
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e2 = {2, 'e'};
    SimplechessSquare e4 = {4, 'e'};
    SimplechessPieceMove move;

    result = simplechess_piece_move_regular(&pawn, &e2, &e4, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_make_move(manager, game, &move, false, &game_after_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Get the stage with the move
    result = simplechess_game_get_stage_at(game_after_move, 1, &stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_stage_get_move(stage, &played_move, &has_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(has_move);

    // Test getting algebraic notation
    result = simplechess_played_move_get_algebraic_notation(played_move, notation_buffer, sizeof(notation_buffer));
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_STR_EQ(notation_buffer, "e4");

    // Test getting piece move
    result = simplechess_played_move_get_piece_move(played_move, &piece_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(piece_move.piece.type, SIMPLECHESS_PIECE_TYPE_PAWN);
    ASSERT_EQ(piece_move.src.rank, 2);
    ASSERT_EQ(piece_move.src.file, 'e');
    ASSERT_EQ(piece_move.dst.rank, 4);
    ASSERT_EQ(piece_move.dst.file, 'e');

    // Test getting captured piece (should be none)
    result = simplechess_played_move_get_captured_piece(played_move, &captured_piece, &has_capture);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!has_capture);

    // Test getting check type
    result = simplechess_played_move_get_check_type(played_move, &check_type);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(check_type, SIMPLECHESS_CHECK_TYPE_NO_CHECK);

    // Test draw offered
    result = simplechess_played_move_is_draw_offered(played_move, &is_draw_offered);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!is_draw_offered);

    simplechess_played_move_destroy(played_move);
    simplechess_game_stage_destroy(stage);
    simplechess_game_destroy(game_after_move);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test board functionality
 */
static int test_board_functionality(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessBoard board;
    SimplechessPiece piece;
    SimplechessSquareAndPiece* squares;
    SimplechessResult result;
    bool has_piece;
    size_t count;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Get current board
    result = simplechess_game_get_current_board(game, &board);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test getting piece at specific squares
    SimplechessSquare e1 = {1, 'e'};
    result = simplechess_board_get_piece_at(board, e1, &piece, &has_piece);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(has_piece);
    ASSERT_EQ(piece.type, SIMPLECHESS_PIECE_TYPE_KING);
    ASSERT_EQ(piece.color, SIMPLECHESS_COLOR_WHITE);

    // Test empty square
    SimplechessSquare e4 = {4, 'e'};
    result = simplechess_board_get_piece_at(board, e4, &piece, &has_piece);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!has_piece);

    // Test getting occupied squares count
    result = simplechess_board_get_occupied_squares_count(board, &count);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(count, 32); // Initial position has 32 pieces

    // Test getting all occupied squares
    squares = malloc(count * sizeof(SimplechessSquareAndPiece));
    ASSERT(squares != NULL);

    result = simplechess_board_get_occupied_squares(board, squares, count);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Verify we got the right number of squares
    // Just check a few key pieces
    int found_white_king = 0;
    int found_black_queen = 0;
    for (size_t i = 0; i < count; i++) {
        if (squares[i].square.rank == 1 && squares[i].square.file == 'e' &&
            squares[i].piece.type == SIMPLECHESS_PIECE_TYPE_KING &&
            squares[i].piece.color == SIMPLECHESS_COLOR_WHITE) {
            found_white_king = 1;
        }
        if (squares[i].square.rank == 8 && squares[i].square.file == 'd' &&
            squares[i].piece.type == SIMPLECHESS_PIECE_TYPE_QUEEN &&
            squares[i].piece.color == SIMPLECHESS_COLOR_BLACK) {
            found_black_queen = 1;
        }
    }
    ASSERT(found_white_king);
    ASSERT(found_black_queen);

    free(squares);
    simplechess_board_destroy(board);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test extended game query functions
 */
static int test_extended_game_queries(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessResult result;
    uint16_t halfmoves, fullmoves;
    uint8_t castling_rights;
    char fen_buffer[100];

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test getting halfmove clock
    result = simplechess_game_get_halfmove_clock(game, &halfmoves);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(halfmoves, 0);

    // Test getting fullmove counter
    result = simplechess_game_get_fullmove_counter(game, &fullmoves);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(fullmoves, 1);

    // Test getting castling rights
    result = simplechess_game_get_castling_rights(game, &castling_rights);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(castling_rights, SIMPLECHESS_CASTLING_WHITE_KINGSIDE | SIMPLECHESS_CASTLING_WHITE_QUEENSIDE |
                              SIMPLECHESS_CASTLING_BLACK_KINGSIDE | SIMPLECHESS_CASTLING_BLACK_QUEENSIDE);

    // Test getting current FEN
    result = simplechess_game_get_current_fen(game, fen_buffer, sizeof(fen_buffer));
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_STR_EQ(fen_buffer, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test additional utility functions
 */
static int test_additional_utilities(void) {
    SimplechessResult result;
    bool is_inside, are_equal;
    SimplechessColor color, opposite;
    SimplechessSquare a1 = {1, 'a'};
    SimplechessSquare a2 = {2, 'a'};
    SimplechessSquare h8 = {8, 'h'};

    // Test square boundary validation
    result = simplechess_square_is_inside_boundaries(1, 'a', &is_inside);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(is_inside);

    result = simplechess_square_is_inside_boundaries(0, 'a', &is_inside);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!is_inside);

    result = simplechess_square_is_inside_boundaries(9, 'a', &is_inside);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!is_inside);

    result = simplechess_square_is_inside_boundaries(1, 'i', &is_inside);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!is_inside);

    // Test square color
    result = simplechess_square_get_color(a1, &color);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(color, SIMPLECHESS_COLOR_BLACK); // a1 is a dark square

    result = simplechess_square_get_color(h8, &color);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(color, SIMPLECHESS_COLOR_BLACK); // h8 is actually a dark square

    // Test square equality
    result = simplechess_squares_are_equal(a1, a1, &are_equal);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(are_equal);

    result = simplechess_squares_are_equal(a1, a2, &are_equal);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(!are_equal);

    // Test color opposite
    result = simplechess_color_get_opposite(SIMPLECHESS_COLOR_WHITE, &opposite);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(opposite, SIMPLECHESS_COLOR_BLACK);

    result = simplechess_color_get_opposite(SIMPLECHESS_COLOR_BLACK, &opposite);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT_EQ(opposite, SIMPLECHESS_COLOR_WHITE);

    return 1;
}

/**
 * Test draw offer functionality
 */
static int test_draw_offer_functionality(void) {
    SimplechessGameManager manager;
    SimplechessGame game, game_after_move;
    SimplechessGameStage stage;
    SimplechessPlayedMove played_move;
    SimplechessResult result;
    bool has_move, is_draw_offered;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Make a move with draw offer
    SimplechessPiece pawn = {SIMPLECHESS_PIECE_TYPE_PAWN, SIMPLECHESS_COLOR_WHITE};
    SimplechessSquare e2 = {2, 'e'};
    SimplechessSquare e4 = {4, 'e'};
    SimplechessPieceMove move;

    result = simplechess_piece_move_regular(&pawn, &e2, &e4, &move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_make_move(manager, game, &move, true, &game_after_move); // With draw offer
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Get the played move and check if draw was offered
    result = simplechess_game_get_stage_at(game_after_move, 1, &stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_stage_get_move(stage, &played_move, &has_move);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(has_move);

    result = simplechess_played_move_is_draw_offered(played_move, &is_draw_offered);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);
    ASSERT(is_draw_offered);

    simplechess_played_move_destroy(played_move);
    simplechess_game_stage_destroy(stage);
    simplechess_game_destroy(game_after_move);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/* ========================================================================== */
/* Error Tests for New Functionality                                         */
/* ========================================================================== */

/**
 * Test null pointer errors for new functions
 */
static int test_new_function_null_errors(void) {
    SimplechessResult result;

    // Test history functions
    result = simplechess_game_get_history_length(NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_game_get_stage_at(NULL, 0, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_game_get_current_stage(NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test stage functions
    result = simplechess_stage_get_board(NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_stage_get_move(NULL, NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_stage_get_active_color(NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test board functions
    result = simplechess_board_get_piece_at(NULL, (SimplechessSquare){1, 'a'}, NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_board_get_occupied_squares_count(NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test played move functions
    result = simplechess_played_move_get_algebraic_notation(NULL, NULL, 0);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_played_move_get_piece_move(NULL, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test utility functions
    result = simplechess_square_is_inside_boundaries(1, 'a', NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_square_get_color((SimplechessSquare){1, 'a'}, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    result = simplechess_color_get_opposite(SIMPLECHESS_COLOR_WHITE, NULL);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    return 1;
}

/**
 * Test index out of range errors
 */
static int test_index_out_of_range_errors(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessGameStage stage;
    SimplechessResult result;

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Try to get stage at invalid index
    result = simplechess_game_get_stage_at(game, 999, &stage);
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/**
 * Test buffer size errors for new functions
 */
static int test_new_function_buffer_errors(void) {
    SimplechessGameManager manager;
    SimplechessGame game;
    SimplechessGameStage stage;
    SimplechessResult result;
    char small_buffer[5]; // Too small for FEN

    result = simplechess_game_manager_create(&manager);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_create_new_game(manager, &game);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    result = simplechess_game_get_current_stage(game, &stage);
    ASSERT_EQ(result, SIMPLECHESS_SUCCESS);

    // Test buffer too small for FEN
    result = simplechess_stage_get_fen(stage, small_buffer, sizeof(small_buffer));
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    // Test buffer too small for current FEN
    result = simplechess_game_get_current_fen(game, small_buffer, sizeof(small_buffer));
    ASSERT_EQ(result, SIMPLECHESS_ERROR_INVALID_ARGUMENT);

    simplechess_game_stage_destroy(stage);
    simplechess_game_destroy(game);
    simplechess_game_manager_destroy(manager);
    return 1;
}

/* ========================================================================== */
/* Main Test Runner                                                           */
/* ========================================================================== */

int main(void) {
    printf("Simple Chess Games C Wrapper Test Suite\n");
    printf("==========================================\n\n");

    /* Success Case Tests */
    printf("=== SUCCESS CASE TESTS ===\n");
    TEST(test_game_manager_lifecycle);
    TEST(test_create_new_game);
    TEST(test_create_game_from_fen);
    TEST(test_square_utilities);
    TEST(test_piece_moves);
    TEST(test_make_moves);
    TEST(test_piece_specific_moves);
    TEST(test_resignation);

    /* Extended API Tests */
    printf("=== EXTENDED API TESTS ===\n");
    TEST(test_game_history);
    TEST(test_game_stage_functionality);
    TEST(test_played_move_functionality);
    TEST(test_board_functionality);
    TEST(test_extended_game_queries);
    TEST(test_additional_utilities);
    TEST(test_draw_offer_functionality);

    /* Error Case Tests */
    printf("=== ERROR CASE TESTS ===\n");
    TEST(test_null_pointer_errors);
    TEST(test_invalid_fen);
    TEST(test_invalid_squares);
    TEST(test_invalid_moves);
    TEST(test_finished_game_operations);
    TEST(test_buffer_overflow_protection);
    TEST(test_array_size_validation);
    TEST(test_draw_reason_error);
    TEST(test_invalid_promotion);
    TEST(test_new_function_null_errors);
    TEST(test_index_out_of_range_errors);
    TEST(test_new_function_buffer_errors);

    /* Utility Tests */
    printf("=== UTILITY TESTS ===\n");
    TEST(test_result_to_string);

    /* Results Summary */
    printf("==========================================\n");
    printf("Test Results:\n");
    printf("  Total:  %d\n", test_count);
    printf("  Passed: %d\n", test_passed);
    printf("  Failed: %d\n", test_failed);
    printf("  Success Rate: %.1f%%\n", (float)test_passed / test_count * 100);

    if (test_failed == 0) {
        printf("\n🎉 All tests passed!\n");
        return 0;
    } else {
        printf("\n💥 Some tests failed!\n");
        return 1;
    }
}