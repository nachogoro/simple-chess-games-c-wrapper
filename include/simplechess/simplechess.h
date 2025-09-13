/**
 * @file simplechess.h
 * @brief C API wrapper for the simple-chess-games C++ library
 *
 * This header provides a C-compatible interface to the simple-chess-games C++
 * library, offering comprehensive chess game functionality with proper error
 * handling and memory management. All C++ exceptions are caught and converted
 * to error codes.
 *
 * @note All functions return SimplechessResult to indicate success or failure.
 * Always check return values before using output parameters.
 *
 * @note Game objects are immutable. Operations that modify game state return
 * new game instances rather than modifying existing ones.
 *
 * @author Simple Chess Games C Wrapper
 * @version 0.0.1
 */

#ifndef SIMPLECHESS_H
#define SIMPLECHESS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Result codes for all API functions
 *
 * All functions in this API return one of these values to indicate
 * the success or failure of the operation.
 */
typedef enum {
    /** @brief Operation completed successfully */
    SIMPLECHESS_SUCCESS = 0,
    /** @brief Invalid argument passed to function */
    SIMPLECHESS_ERROR_INVALID_ARGUMENT = 1,
    /** @brief Operation attempted on game in invalid state */
    SIMPLECHESS_ERROR_ILLEGAL_STATE = 2,
    /** @brief Memory allocation failed */
    SIMPLECHESS_ERROR_OUT_OF_MEMORY = 3,
    /** @brief Unknown or unexpected error occurred */
    SIMPLECHESS_ERROR_UNKNOWN = 4
} SimplechessResult;

/**
 * @brief Chess piece colors
 */
typedef enum {
    /** @brief White pieces */
    SIMPLECHESS_COLOR_WHITE = 0,
    /** @brief Black pieces */
    SIMPLECHESS_COLOR_BLACK = 1
} SimplechessColor;

/**
 * @brief Chess game states
 */
typedef enum {
    /** @brief Game is still in progress */
    SIMPLECHESS_GAME_STATE_PLAYING = 0,
    /** @brief Game ended in a draw */
    SIMPLECHESS_GAME_STATE_DRAWN = 1,
    /** @brief White player won */
    SIMPLECHESS_GAME_STATE_WHITE_WON = 2,
    /** @brief Black player won */
    SIMPLECHESS_GAME_STATE_BLACK_WON = 3
} SimplechessGameState;

/**
 * @brief Types of chess pieces
 */
typedef enum {
    /** @brief Pawn */
    SIMPLECHESS_PIECE_TYPE_PAWN = 0,
    /** @brief Rook */
    SIMPLECHESS_PIECE_TYPE_ROOK = 1,
    /** @brief Knight */
    SIMPLECHESS_PIECE_TYPE_KNIGHT = 2,
    /** @brief Bishop */
    SIMPLECHESS_PIECE_TYPE_BISHOP = 3,
    /** @brief Queen */
    SIMPLECHESS_PIECE_TYPE_QUEEN = 4,
    /** @brief King */
    SIMPLECHESS_PIECE_TYPE_KING = 5
} SimplechessPieceType;

/**
 * @brief Reasons why a game might end in a draw
 */
typedef enum {
    /** @brief Stalemate (no legal moves, not in check) */
    SIMPLECHESS_DRAW_REASON_STALEMATE = 0,
    /** @brief Neither side has sufficient material to checkmate */
    SIMPLECHESS_DRAW_REASON_INSUFFICIENT_MATERIAL = 1,
    /** @brief Draw offer was made and accepted */
    SIMPLECHESS_DRAW_REASON_OFFERED_AND_ACCEPTED = 2,
    /** @brief Same position occurred three times */
    SIMPLECHESS_DRAW_REASON_THREE_FOLD_REPETITION = 3,
    /** @brief Same position occurred five times */
    SIMPLECHESS_DRAW_REASON_FIVE_FOLD_REPETITION = 4,
    /** @brief Fifty moves without capture or pawn move */
    SIMPLECHESS_DRAW_REASON_FIFTY_MOVE_RULE = 5,
    /** @brief Seventy-five moves without capture or pawn move */
    SIMPLECHESS_DRAW_REASON_SEVENTY_FIVE_MOVE_RULE = 6
} SimplechessDrawReason;

/**
 * @brief Check status in chess
 */
typedef enum {
    /** @brief Not in check */
    SIMPLECHESS_CHECK_TYPE_NO_CHECK = 0,
    /** @brief In check */
    SIMPLECHESS_CHECK_TYPE_CHECK = 1,
    /** @brief Checkmate */
    SIMPLECHESS_CHECK_TYPE_CHECKMATE = 2
} SimplechessCheckType;

/**
 * @brief Castling rights flags
 */
typedef enum {
    /** @brief White kingside castling */
    SIMPLECHESS_CASTLING_WHITE_KINGSIDE = 1,
    /** @brief White queenside castling */
    SIMPLECHESS_CASTLING_WHITE_QUEENSIDE = 2,
    /** @brief Black kingside castling */
    SIMPLECHESS_CASTLING_BLACK_KINGSIDE = 4,
    /** @brief Black queenside castling */
    SIMPLECHESS_CASTLING_BLACK_QUEENSIDE = 8
} SimplechessCastlingRight;

/**
 * @brief Represents a square on the chess board
 */
typedef struct {
    /** @brief Rank (1-8) */
    uint8_t rank;
    /** @brief File ('a'-'h') */
    char file;
} SimplechessSquare;

/**
 * @brief Represents a chess piece
 */
typedef struct {
    /** @brief Type of the piece */
    SimplechessPieceType type;
    /** @brief Color of the piece */
    SimplechessColor color;
} SimplechessPiece;

/**
 * @brief Represents a chess move
 */
typedef struct {
    /** @brief The piece being moved */
    SimplechessPiece piece;
    /** @brief Source square */
    SimplechessSquare src;
    /** @brief Destination square */
    SimplechessSquare dst;
    /** @brief Type of piece for pawn promotion */
    SimplechessPieceType promoted_type;
    /** @brief True if this is a pawn promotion move */
    bool is_promotion;
} SimplechessPieceMove;

/**
 * @brief Represents a square with a piece on it
 */
typedef struct {
    /** @brief The square */
    SimplechessSquare square;
    /** @brief The piece on the square */
    SimplechessPiece piece;
} SimplechessSquareAndPiece;

/**
 * @brief Opaque handle to a game manager
 *
 * The game manager is responsible for creating games and executing moves.
 * It must be created with simplechess_game_manager_create() and destroyed
 * with simplechess_game_manager_destroy().
 */
typedef void* SimplechessGameManager;

/**
 * @brief Opaque handle to a game instance
 *
 * Games are immutable objects representing the state of a chess game at
 * a specific point. They must be destroyed with simplechess_game_destroy().
 */
typedef void* SimplechessGame;

/**
 * @brief Opaque handle to a game stage
 *
 * A game stage represents the state of the game at a specific move.
 * They must be destroyed with simplechess_game_stage_destroy().
 */
typedef void* SimplechessGameStage;

/**
 * @brief Opaque handle to a played move
 *
 * A played move contains information about a move that was made in the game,
 * including algebraic notation and captured pieces.
 * They must be destroyed with simplechess_played_move_destroy().
 */
typedef void* SimplechessPlayedMove;

/**
 * @brief Opaque handle to a board
 *
 * A board represents the piece positions at a specific point in the game.
 * They must be destroyed with simplechess_board_destroy().
 */
typedef void* SimplechessBoard;

/* ========================================================================== */
/* Game Manager Functions                                                     */
/* ========================================================================== */

/**
 * @brief Create a new game manager
 *
 * Creates a new game manager instance that can be used to create and
 * manipulate chess games. The manager must be destroyed with
 * simplechess_game_manager_destroy() when no longer needed.
 *
 * @param[out] manager Pointer to store the created manager handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if manager is NULL
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_game_manager_create(SimplechessGameManager* manager);

/**
 * @brief Destroy a game manager
 *
 * Destroys a game manager and releases all associated resources.
 * After calling this function, the manager handle becomes invalid.
 *
 * @param manager Manager handle to destroy (can be NULL)
 */
void simplechess_game_manager_destroy(SimplechessGameManager manager);

/* ========================================================================== */
/* Game Creation Functions                                                    */
/* ========================================================================== */

/**
 * @brief Create a new game from the standard starting position
 *
 * Creates a new chess game in the standard starting position with
 * white to move first.
 *
 * @param manager Game manager handle
 * @param[out] game Pointer to store the created game handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if manager or game is NULL
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_create_new_game(SimplechessGameManager manager, SimplechessGame* game);

/**
 * @brief Create a game from a FEN (Forsyth-Edwards Notation) string
 *
 * Creates a chess game from the position described by the FEN string.
 *
 * @note FEN strings only provide limited historical information, so some
 * drawing rules (like threefold repetition) may not be enforceable.
 *
 * @param manager Game manager handle
 * @param fen FEN string describing the position
 * @param[out] game Pointer to store the created game handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or FEN is invalid
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_create_game_from_fen(SimplechessGameManager manager, const char* fen, SimplechessGame* game);

/* ========================================================================== */
/* Game Manipulation Functions                                               */
/* ========================================================================== */

/**
 * @brief Make a move in the game
 *
 * Attempts to make the specified move for the current active player.
 * Returns a new game instance with the move applied.
 *
 * @param manager Game manager handle
 * @param input_game Current game state
 * @param move The move to make
 * @param offer_draw True if the move includes a draw offer
 * @param[out] result_game Pointer to store the new game state
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_ILLEGAL_STATE if game is over or move is invalid
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_make_move(
    SimplechessGameManager manager,
    SimplechessGame input_game,
    const SimplechessPieceMove* move,
    bool offer_draw,
    SimplechessGame* result_game);

/**
 * @brief Claim a draw
 *
 * Claims a draw for the current active player. A draw can only be claimed
 * under specific circumstances (see simplechess_game_can_claim_draw).
 *
 * @param manager Game manager handle
 * @param input_game Current game state
 * @param[out] result_game Pointer to store the drawn game state
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_ILLEGAL_STATE if game is over or draw cannot be claimed
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_claim_draw(SimplechessGameManager manager, SimplechessGame input_game, SimplechessGame* result_game);

/**
 * @brief Resign the game
 *
 * Resigns the game for the specified player, ending the game with
 * a victory for the opponent.
 *
 * @param manager Game manager handle
 * @param input_game Current game state
 * @param resigning_player The player who is resigning
 * @param[out] result_game Pointer to store the final game state
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_ILLEGAL_STATE if game is already over
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_resign(SimplechessGameManager manager, SimplechessGame input_game, SimplechessColor resigning_player, SimplechessGame* result_game);

/* ========================================================================== */
/* Game Query Functions                                                       */
/* ========================================================================== */

/**
 * @brief Get the current state of the game
 *
 * Returns whether the game is still in progress or has ended, and if
 * ended, how it ended.
 *
 * @param game Game handle
 * @param[out] state Pointer to store the game state
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_state(SimplechessGame game, SimplechessGameState* state);

/**
 * @brief Get the reason why the game was drawn
 *
 * Returns the specific reason for the draw. Only valid if the game
 * state is SIMPLECHESS_GAME_STATE_DRAWN.
 *
 * @param game Game handle
 * @param[out] reason Pointer to store the draw reason
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_ILLEGAL_STATE if game was not drawn
 */
SimplechessResult simplechess_game_get_draw_reason(SimplechessGame game, SimplechessDrawReason* reason);

/**
 * @brief Get the color of the player whose turn it is
 *
 * Returns which player (white or black) is to move next.
 *
 * @param game Game handle
 * @param[out] color Pointer to store the active color
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_active_color(SimplechessGame game, SimplechessColor* color);

/**
 * @brief Check if a draw can be claimed and get the reason
 *
 * Determines whether the current active player can claim a draw and
 * provides the reason if applicable.
 *
 * @param game Game handle
 * @param[out] can_claim Pointer to store whether draw can be claimed
 * @param[out] reason Pointer to store draw reason (can be NULL)
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if game or can_claim is NULL
 * @retval SIMPLECHESS_ERROR_ILLEGAL_STATE if game is not in playing state
 */
SimplechessResult simplechess_game_can_claim_draw(SimplechessGame game, bool* can_claim, SimplechessDrawReason* reason);

/* ========================================================================== */
/* Move Query Functions                                                       */
/* ========================================================================== */

/**
 * @brief Get the number of available moves for the current player
 *
 * Returns the total count of legal moves available to the active player.
 *
 * @param game Game handle
 * @param[out] count Pointer to store the move count
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_available_moves_count(SimplechessGame game, size_t* count);

/**
 * @brief Get all available moves for the current player
 *
 * Fills the provided array with all legal moves for the active player.
 * Use simplechess_game_get_available_moves_count() first to determine
 * the required array size.
 *
 * @param game Game handle
 * @param[out] moves Array to store the moves
 * @param moves_size Size of the moves array
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or array too small
 */
SimplechessResult simplechess_game_get_available_moves(SimplechessGame game, SimplechessPieceMove* moves, size_t moves_size);

/**
 * @brief Get the number of moves available for a specific piece
 *
 * Returns the count of legal moves for the piece at the specified square.
 * Returns 0 if the square is empty or contains an opponent's piece.
 *
 * @param game Game handle
 * @param square The square to query
 * @param[out] count Pointer to store the move count
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_moves_for_piece_count(SimplechessGame game, const SimplechessSquare* square, size_t* count);

/**
 * @brief Get all moves available for a specific piece
 *
 * Fills the provided array with all legal moves for the piece at the
 * specified square. Use simplechess_game_get_moves_for_piece_count()
 * first to determine the required array size.
 *
 * @param game Game handle
 * @param square The square to query
 * @param[out] moves Array to store the moves
 * @param moves_size Size of the moves array
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or array too small
 */
SimplechessResult simplechess_game_get_moves_for_piece(SimplechessGame game, const SimplechessSquare* square, SimplechessPieceMove* moves, size_t moves_size);

/* ========================================================================== */
/* Game History Functions                                                     */
/* ========================================================================== */

/**
 * @brief Get the number of stages in the game history
 *
 * Returns the total number of game stages (moves + initial position).
 * The initial position is always at index 0.
 *
 * @param game Game handle
 * @param[out] length Pointer to store the history length
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_history_length(SimplechessGame game, size_t* length);

/**
 * @brief Get a specific stage from the game history
 *
 * Returns the game stage at the specified index. Index 0 is the initial
 * position, and subsequent indices represent positions after each move.
 *
 * @param game Game handle
 * @param index Index of the stage (0-based)
 * @param[out] stage Pointer to store the stage handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or index out of range
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_game_get_stage_at(SimplechessGame game, size_t index, SimplechessGameStage* stage);

/**
 * @brief Get the current stage of the game
 *
 * Returns the current (most recent) stage of the game, equivalent to
 * calling simplechess_game_get_stage_at with the last index.
 *
 * @param game Game handle
 * @param[out] stage Pointer to store the stage handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_game_get_current_stage(SimplechessGame game, SimplechessGameStage* stage);

/* ========================================================================== */
/* Game Stage Functions                                                       */
/* ========================================================================== */

/**
 * @brief Get the board from a game stage
 *
 * Returns the board state at the specified game stage.
 *
 * @param stage Game stage handle
 * @param[out] board Pointer to store the board handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_stage_get_board(SimplechessGameStage stage, SimplechessBoard* board);

/**
 * @brief Get the move that led to this stage
 *
 * Returns the move that was played to reach this game stage.
 * The initial position (index 0) has no move.
 *
 * @param stage Game stage handle
 * @param[out] move Pointer to store the played move handle
 * @param[out] has_move Pointer to store whether a move exists
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_stage_get_move(SimplechessGameStage stage, SimplechessPlayedMove* move, bool* has_move);

/**
 * @brief Get the active color at this stage
 *
 * Returns which player's turn it is at this game stage.
 *
 * @param stage Game stage handle
 * @param[out] color Pointer to store the active color
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_stage_get_active_color(SimplechessGameStage stage, SimplechessColor* color);

/**
 * @brief Get the castling rights at this stage
 *
 * Returns the castling rights bitfield at this game stage.
 *
 * @param stage Game stage handle
 * @param[out] rights Pointer to store the castling rights
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_stage_get_castling_rights(SimplechessGameStage stage, uint8_t* rights);

/**
 * @brief Get the halfmove clock at this stage
 *
 * Returns the number of halfmoves since the last capture or pawn advance.
 *
 * @param stage Game stage handle
 * @param[out] halfmoves Pointer to store the halfmove clock
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_stage_get_halfmove_clock(SimplechessGameStage stage, uint16_t* halfmoves);

/**
 * @brief Get the fullmove counter at this stage
 *
 * Returns the fullmove counter (increments after each black move).
 *
 * @param stage Game stage handle
 * @param[out] fullmoves Pointer to store the fullmove counter
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_stage_get_fullmove_counter(SimplechessGameStage stage, uint16_t* fullmoves);

/**
 * @brief Get the FEN string for this stage
 *
 * Returns the FEN (Forsyth-Edwards Notation) string representing
 * the position at this game stage.
 *
 * @param stage Game stage handle
 * @param[out] buffer Buffer to store the FEN string
 * @param buffer_size Size of the buffer
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or buffer too small
 */
SimplechessResult simplechess_stage_get_fen(SimplechessGameStage stage, char* buffer, size_t buffer_size);

/* ========================================================================== */
/* Played Move Functions                                                      */
/* ========================================================================== */

/**
 * @brief Get the algebraic notation for a played move
 *
 * Returns the algebraic notation string (e.g., "Nf3", "Bxc4+") for the move.
 *
 * @param move Played move handle
 * @param[out] buffer Buffer to store the notation string
 * @param buffer_size Size of the buffer
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or buffer too small
 */
SimplechessResult simplechess_played_move_get_algebraic_notation(SimplechessPlayedMove move, char* buffer, size_t buffer_size);

/**
 * @brief Get the piece move from a played move
 *
 * Returns the underlying piece move information.
 *
 * @param move Played move handle
 * @param[out] piece_move Pointer to store the piece move
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_played_move_get_piece_move(SimplechessPlayedMove move, SimplechessPieceMove* piece_move);

/**
 * @brief Get the captured piece from a played move
 *
 * Returns the piece that was captured by this move, if any.
 *
 * @param move Played move handle
 * @param[out] piece Pointer to store the captured piece
 * @param[out] has_capture Pointer to store whether a piece was captured
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_played_move_get_captured_piece(SimplechessPlayedMove move, SimplechessPiece* piece, bool* has_capture);

/**
 * @brief Get the check type from a played move
 *
 * Returns whether this move resulted in check, checkmate, or neither.
 *
 * @param move Played move handle
 * @param[out] check_type Pointer to store the check type
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_played_move_get_check_type(SimplechessPlayedMove move, SimplechessCheckType* check_type);

/**
 * @brief Check if a draw was offered with this move
 *
 * Returns whether a draw offer was made with this move.
 *
 * @param move Played move handle
 * @param[out] is_offered Pointer to store whether draw was offered
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_played_move_is_draw_offered(SimplechessPlayedMove move, bool* is_offered);

/* ========================================================================== */
/* Board Functions                                                            */
/* ========================================================================== */

/**
 * @brief Get the piece at a specific square
 *
 * Returns the piece at the specified square, if any.
 *
 * @param board Board handle
 * @param square The square to query
 * @param[out] piece Pointer to store the piece
 * @param[out] has_piece Pointer to store whether a piece exists at the square
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_board_get_piece_at(SimplechessBoard board, SimplechessSquare square, SimplechessPiece* piece, bool* has_piece);

/**
 * @brief Get the number of occupied squares on the board
 *
 * Returns the total number of squares that have pieces on them.
 *
 * @param board Board handle
 * @param[out] count Pointer to store the count
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_board_get_occupied_squares_count(SimplechessBoard board, size_t* count);

/**
 * @brief Get all occupied squares and their pieces
 *
 * Fills the provided array with all occupied squares and their pieces.
 * Use simplechess_board_get_occupied_squares_count() first to determine
 * the required array size.
 *
 * @param board Board handle
 * @param[out] squares Array to store the squares and pieces
 * @param array_size Size of the squares array
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or array too small
 */
SimplechessResult simplechess_board_get_occupied_squares(SimplechessBoard board, SimplechessSquareAndPiece* squares, size_t array_size);

/* ========================================================================== */
/* Extended Game Query Functions                                              */
/* ========================================================================== */

/**
 * @brief Get the halfmove clock from the current game
 *
 * Returns the number of halfmoves since the last capture or pawn advance.
 *
 * @param game Game handle
 * @param[out] halfmoves Pointer to store the halfmove clock
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_halfmove_clock(SimplechessGame game, uint16_t* halfmoves);

/**
 * @brief Get the fullmove counter from the current game
 *
 * Returns the fullmove counter (increments after each black move).
 *
 * @param game Game handle
 * @param[out] fullmoves Pointer to store the fullmove counter
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_fullmove_counter(SimplechessGame game, uint16_t* fullmoves);

/**
 * @brief Get the current FEN string for the game
 *
 * Returns the FEN (Forsyth-Edwards Notation) string representing
 * the current position.
 *
 * @param game Game handle
 * @param[out] buffer Buffer to store the FEN string
 * @param buffer_size Size of the buffer
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or buffer too small
 */
SimplechessResult simplechess_game_get_current_fen(SimplechessGame game, char* buffer, size_t buffer_size);

/**
 * @brief Get the castling rights from the current game
 *
 * Returns the castling rights bitfield for the current position.
 *
 * @param game Game handle
 * @param[out] rights Pointer to store the castling rights
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_game_get_castling_rights(SimplechessGame game, uint8_t* rights);

/**
 * @brief Get the current board from the game
 *
 * Returns the current board state of the game.
 *
 * @param game Game handle
 * @param[out] board Pointer to store the board handle
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 * @retval SIMPLECHESS_ERROR_OUT_OF_MEMORY if allocation fails
 */
SimplechessResult simplechess_game_get_current_board(SimplechessGame game, SimplechessBoard* board);

/* ========================================================================== */
/* Utility Functions                                                          */
/* ========================================================================== */

/**
 * @brief Create a square from rank and file
 *
 * Creates a square representation from numeric rank (1-8) and
 * alphabetic file ('a'-'h', case insensitive).
 *
 * @param rank Rank number (1-8)
 * @param file File letter ('a'-'h', case insensitive)
 * @param[out] square Pointer to store the created square
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if square is NULL or rank/file out of range
 */
SimplechessResult simplechess_square_from_rank_and_file(uint8_t rank, char file, SimplechessSquare* square);

/**
 * @brief Create a square from algebraic notation
 *
 * Parses a square from algebraic notation string (e.g., "e4", "a1").
 * The parsing is case insensitive.
 *
 * @param algebraic Algebraic notation string
 * @param[out] square Pointer to store the created square
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or string invalid
 */
SimplechessResult simplechess_square_from_string(const char* algebraic, SimplechessSquare* square);

/**
 * @brief Convert a square to string representation
 *
 * Converts a square to its algebraic notation string (e.g., "e4").
 * The buffer must be at least 3 characters (2 for notation + null terminator).
 *
 * @param square The square to convert
 * @param[out] buffer Buffer to store the string
 * @param buffer_size Size of the buffer (must be >= 3)
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL or buffer too small
 */
SimplechessResult simplechess_square_to_string(const SimplechessSquare* square, char* buffer, size_t buffer_size);

/**
 * @brief Create a regular piece move
 *
 * Creates a move for any piece that doesn't involve pawn promotion.
 * For castling moves, specify the king's source and destination squares.
 *
 * @param piece The piece being moved
 * @param src Source square
 * @param dst Destination square
 * @param[out] move Pointer to store the created move
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_piece_move_regular(const SimplechessPiece* piece, const SimplechessSquare* src, const SimplechessSquare* dst, SimplechessPieceMove* move);

/**
 * @brief Create a pawn promotion move
 *
 * Creates a move for a pawn that promotes to another piece type.
 * The piece parameter must be a pawn, and promoted_type must be
 * a valid promotion target (rook, knight, bishop, or queen).
 *
 * @param piece The pawn being moved (must be PAWN type)
 * @param src Source square
 * @param dst Destination square
 * @param promoted_type Type of piece to promote to
 * @param[out] move Pointer to store the created move
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL, piece is not pawn, or invalid promotion type
 */
SimplechessResult simplechess_piece_move_promotion(const SimplechessPiece* piece, const SimplechessSquare* src, const SimplechessSquare* dst, SimplechessPieceType promoted_type, SimplechessPieceMove* move);

/**
 * @brief Check if a square is within board boundaries
 *
 * Validates whether the given rank and file represent a valid chess square.
 *
 * @param rank Rank number to check
 * @param file File letter to check
 * @param[out] is_inside Pointer to store the result
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if is_inside is NULL
 */
SimplechessResult simplechess_square_is_inside_boundaries(uint8_t rank, char file, bool* is_inside);

/**
 * @brief Get the color of a square
 *
 * Returns whether a square is a light or dark square on the chessboard.
 *
 * @param square The square to check
 * @param[out] color Pointer to store the square color (white for light, black for dark)
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if any parameter is NULL
 */
SimplechessResult simplechess_square_get_color(SimplechessSquare square, SimplechessColor* color);

/**
 * @brief Compare two squares for equality
 *
 * Checks if two squares represent the same position on the board.
 *
 * @param a First square to compare
 * @param b Second square to compare
 * @param[out] are_equal Pointer to store the comparison result
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if are_equal is NULL
 */
SimplechessResult simplechess_squares_are_equal(SimplechessSquare a, SimplechessSquare b, bool* are_equal);

/**
 * @brief Get the opposite color
 *
 * Returns the opposite chess piece color (white -> black, black -> white).
 *
 * @param color The input color
 * @param[out] opposite Pointer to store the opposite color
 * @return SIMPLECHESS_SUCCESS on success, error code on failure
 *
 * @retval SIMPLECHESS_ERROR_INVALID_ARGUMENT if opposite is NULL
 */
SimplechessResult simplechess_color_get_opposite(SimplechessColor color, SimplechessColor* opposite);

/* ========================================================================== */
/* Memory Management Functions                                                */
/* ========================================================================== */

/**
 * @brief Destroy a game instance
 *
 * Destroys a game instance and releases all associated resources.
 * After calling this function, the game handle becomes invalid.
 *
 * @param game Game handle to destroy (can be NULL)
 */
void simplechess_game_destroy(SimplechessGame game);

/**
 * @brief Destroy a game stage instance
 *
 * Destroys a game stage instance and releases all associated resources.
 * After calling this function, the stage handle becomes invalid.
 *
 * @param stage Game stage handle to destroy (can be NULL)
 */
void simplechess_game_stage_destroy(SimplechessGameStage stage);

/**
 * @brief Destroy a played move instance
 *
 * Destroys a played move instance and releases all associated resources.
 * After calling this function, the move handle becomes invalid.
 *
 * @param move Played move handle to destroy (can be NULL)
 */
void simplechess_played_move_destroy(SimplechessPlayedMove move);

/**
 * @brief Destroy a board instance
 *
 * Destroys a board instance and releases all associated resources.
 * After calling this function, the board handle becomes invalid.
 *
 * @param board Board handle to destroy (can be NULL)
 */
void simplechess_board_destroy(SimplechessBoard board);

/* ========================================================================== */
/* Error Handling Functions                                                   */
/* ========================================================================== */

/**
 * @brief Convert result code to human-readable string
 *
 * Returns a static string describing the given result code.
 * The returned string should not be freed.
 *
 * @param result Result code to convert
 * @return Human-readable description of the result
 */
const char* simplechess_result_to_string(SimplechessResult result);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLECHESS_H */
