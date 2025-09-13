#include "simplechess/simplechess.h"
#include <simplechess/GameManager.h>
#include <simplechess/Game.h>
#include <simplechess/GameStage.h>
#include <simplechess/PlayedMove.h>
#include <simplechess/Board.h>
#include <simplechess/PieceMove.h>
#include <simplechess/Square.h>
#include <simplechess/Piece.h>
#include <simplechess/Color.h>
#include <simplechess/Exceptions.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstring>
#include <map>

namespace {
    SimplechessColor cpp_to_c_color(simplechess::Color color) {
        return color == simplechess::Color::White ? SIMPLECHESS_COLOR_WHITE : SIMPLECHESS_COLOR_BLACK;
    }

    simplechess::Color c_to_cpp_color(SimplechessColor color) {
        return color == SIMPLECHESS_COLOR_WHITE ? simplechess::Color::White : simplechess::Color::Black;
    }

    SimplechessGameState cpp_to_c_game_state(simplechess::GameState state) {
        switch (state) {
            case simplechess::GameState::Playing: return SIMPLECHESS_GAME_STATE_PLAYING;
            case simplechess::GameState::Drawn: return SIMPLECHESS_GAME_STATE_DRAWN;
            case simplechess::GameState::WhiteWon: return SIMPLECHESS_GAME_STATE_WHITE_WON;
            case simplechess::GameState::BlackWon: return SIMPLECHESS_GAME_STATE_BLACK_WON;
        }
        return SIMPLECHESS_GAME_STATE_PLAYING;
    }

    SimplechessPieceType cpp_to_c_piece_type(simplechess::PieceType type) {
        switch (type) {
            case simplechess::PieceType::Pawn: return SIMPLECHESS_PIECE_TYPE_PAWN;
            case simplechess::PieceType::Rook: return SIMPLECHESS_PIECE_TYPE_ROOK;
            case simplechess::PieceType::Knight: return SIMPLECHESS_PIECE_TYPE_KNIGHT;
            case simplechess::PieceType::Bishop: return SIMPLECHESS_PIECE_TYPE_BISHOP;
            case simplechess::PieceType::Queen: return SIMPLECHESS_PIECE_TYPE_QUEEN;
            case simplechess::PieceType::King: return SIMPLECHESS_PIECE_TYPE_KING;
        }
        return SIMPLECHESS_PIECE_TYPE_PAWN;
    }

    simplechess::PieceType c_to_cpp_piece_type(SimplechessPieceType type) {
        switch (type) {
            case SIMPLECHESS_PIECE_TYPE_PAWN: return simplechess::PieceType::Pawn;
            case SIMPLECHESS_PIECE_TYPE_ROOK: return simplechess::PieceType::Rook;
            case SIMPLECHESS_PIECE_TYPE_KNIGHT: return simplechess::PieceType::Knight;
            case SIMPLECHESS_PIECE_TYPE_BISHOP: return simplechess::PieceType::Bishop;
            case SIMPLECHESS_PIECE_TYPE_QUEEN: return simplechess::PieceType::Queen;
            case SIMPLECHESS_PIECE_TYPE_KING: return simplechess::PieceType::King;
        }
        return simplechess::PieceType::Pawn;
    }

    SimplechessDrawReason cpp_to_c_draw_reason(simplechess::DrawReason reason) {
        switch (reason) {
            case simplechess::DrawReason::StaleMate: return SIMPLECHESS_DRAW_REASON_STALEMATE;
            case simplechess::DrawReason::InsufficientMaterial: return SIMPLECHESS_DRAW_REASON_INSUFFICIENT_MATERIAL;
            case simplechess::DrawReason::OfferedAndAccepted: return SIMPLECHESS_DRAW_REASON_OFFERED_AND_ACCEPTED;
            case simplechess::DrawReason::ThreeFoldRepetition: return SIMPLECHESS_DRAW_REASON_THREE_FOLD_REPETITION;
            case simplechess::DrawReason::FiveFoldRepetition: return SIMPLECHESS_DRAW_REASON_FIVE_FOLD_REPETITION;
            case simplechess::DrawReason::FiftyMoveRule: return SIMPLECHESS_DRAW_REASON_FIFTY_MOVE_RULE;
            case simplechess::DrawReason::SeventyFiveMoveRule: return SIMPLECHESS_DRAW_REASON_SEVENTY_FIVE_MOVE_RULE;
        }
        return SIMPLECHESS_DRAW_REASON_STALEMATE;
    }

    SimplechessSquare cpp_to_c_square(const simplechess::Square& square) {
        SimplechessSquare result;
        result.rank = square.rank();
        result.file = square.file();
        return result;
    }

    simplechess::Square c_to_cpp_square(const SimplechessSquare& square) {
        return simplechess::Square::fromRankAndFile(square.rank, square.file);
    }

    SimplechessPiece cpp_to_c_piece(const simplechess::Piece& piece) {
        SimplechessPiece result;
        result.type = cpp_to_c_piece_type(piece.type());
        result.color = cpp_to_c_color(piece.color());
        return result;
    }

    simplechess::Piece c_to_cpp_piece(const SimplechessPiece& piece) {
        return simplechess::Piece(c_to_cpp_piece_type(piece.type), c_to_cpp_color(piece.color));
    }

    SimplechessPieceMove cpp_to_c_piece_move(const simplechess::PieceMove& move) {
        SimplechessPieceMove result;
        result.piece = cpp_to_c_piece(move.piece());
        result.src = cpp_to_c_square(move.src());
        result.dst = cpp_to_c_square(move.dst());
        result.is_promotion = move.promoted().has_value();
        result.promoted_type = result.is_promotion ? cpp_to_c_piece_type(move.promoted().value()) : SIMPLECHESS_PIECE_TYPE_PAWN;
        return result;
    }

    simplechess::PieceMove c_to_cpp_piece_move(const SimplechessPieceMove& move) {
        if (move.is_promotion) {
            return simplechess::PieceMove::pawnPromotion(
                c_to_cpp_piece(move.piece),
                c_to_cpp_square(move.src),
                c_to_cpp_square(move.dst),
                c_to_cpp_piece_type(move.promoted_type)
            );
        } else {
            return simplechess::PieceMove::regularMove(
                c_to_cpp_piece(move.piece),
                c_to_cpp_square(move.src),
                c_to_cpp_square(move.dst)
            );
        }
    }

    SimplechessCheckType cpp_to_c_check_type(simplechess::CheckType check_type) {
        switch (check_type) {
            case simplechess::CheckType::NoCheck: return SIMPLECHESS_CHECK_TYPE_NO_CHECK;
            case simplechess::CheckType::Check: return SIMPLECHESS_CHECK_TYPE_CHECK;
            case simplechess::CheckType::CheckMate: return SIMPLECHESS_CHECK_TYPE_CHECKMATE;
        }
        return SIMPLECHESS_CHECK_TYPE_NO_CHECK;
    }

    SimplechessSquareAndPiece cpp_to_c_square_and_piece(const std::pair<simplechess::Square, simplechess::Piece>& pair) {
        SimplechessSquareAndPiece result;
        result.square = cpp_to_c_square(pair.first);
        result.piece = cpp_to_c_piece(pair.second);
        return result;
    }

    SimplechessResult handle_exception() {
        try {
            throw;
        } catch (const simplechess::IllegalStateException&) {
            return SIMPLECHESS_ERROR_ILLEGAL_STATE;
        } catch (const std::invalid_argument&) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        } catch (const std::out_of_range&) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        } catch (const std::bad_alloc&) {
            return SIMPLECHESS_ERROR_OUT_OF_MEMORY;
        } catch (...) {
            return SIMPLECHESS_ERROR_UNKNOWN;
        }
    }
}

extern "C" {

SimplechessResult simplechess_game_manager_create(SimplechessGameManager* manager) {
    if (!manager) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        *manager = new simplechess::GameManager();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

void simplechess_game_manager_destroy(SimplechessGameManager manager) {
    if (manager) {
        delete static_cast<simplechess::GameManager*>(manager);
    }
}

SimplechessResult simplechess_create_new_game(SimplechessGameManager manager, SimplechessGame* game) {
    if (!manager || !game) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* mgr = static_cast<simplechess::GameManager*>(manager);
        auto new_game = mgr->createNewGame();
        *game = new simplechess::Game(std::move(new_game));
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_create_game_from_fen(SimplechessGameManager manager, const char* fen, SimplechessGame* game) {
    if (!manager || !fen || !game) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* mgr = static_cast<simplechess::GameManager*>(manager);
        auto new_game = mgr->createGameFromFen(std::string(fen));
        *game = new simplechess::Game(std::move(new_game));
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_make_move(SimplechessGameManager manager, SimplechessGame input_game, const SimplechessPieceMove* move, bool offer_draw, SimplechessGame* result_game) {
    if (!manager || !input_game || !move || !result_game) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* mgr = static_cast<simplechess::GameManager*>(manager);
        auto* game = static_cast<simplechess::Game*>(input_game);
        auto cpp_move = c_to_cpp_piece_move(*move);
        auto new_game = mgr->makeMove(*game, cpp_move, offer_draw);
        *result_game = new simplechess::Game(std::move(new_game));
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_claim_draw(SimplechessGameManager manager, SimplechessGame input_game, SimplechessGame* result_game) {
    if (!manager || !input_game || !result_game) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* mgr = static_cast<simplechess::GameManager*>(manager);
        auto* game = static_cast<simplechess::Game*>(input_game);
        auto new_game = mgr->claimDraw(*game);
        *result_game = new simplechess::Game(std::move(new_game));
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_resign(SimplechessGameManager manager, SimplechessGame input_game, SimplechessColor resigning_player, SimplechessGame* result_game) {
    if (!manager || !input_game || !result_game) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* mgr = static_cast<simplechess::GameManager*>(manager);
        auto* game = static_cast<simplechess::Game*>(input_game);
        auto cpp_color = c_to_cpp_color(resigning_player);
        auto new_game = mgr->resign(*game, cpp_color);
        *result_game = new simplechess::Game(std::move(new_game));
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_state(SimplechessGame game, SimplechessGameState* state) {
    if (!game || !state) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *state = cpp_to_c_game_state(g->gameState());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_draw_reason(SimplechessGame game, SimplechessDrawReason* reason) {
    if (!game || !reason) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *reason = cpp_to_c_draw_reason(g->drawReason());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_active_color(SimplechessGame game, SimplechessColor* color) {
    if (!game || !color) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *color = cpp_to_c_color(g->activeColor());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_can_claim_draw(SimplechessGame game, bool* can_claim, SimplechessDrawReason* reason) {
    if (!game || !can_claim) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        auto draw_reason = g->reasonToClaimDraw();
        *can_claim = draw_reason.has_value();
        if (*can_claim && reason) {
            *reason = cpp_to_c_draw_reason(draw_reason.value());
        }
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_available_moves_count(SimplechessGame game, size_t* count) {
    if (!game || !count) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *count = g->allAvailableMoves().size();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_available_moves(SimplechessGame game, SimplechessPieceMove* moves, size_t moves_size) {
    if (!game || !moves) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        const auto& cpp_moves = g->allAvailableMoves();

        if (moves_size < cpp_moves.size()) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }

        size_t i = 0;
        for (const auto& move : cpp_moves) {
            moves[i++] = cpp_to_c_piece_move(move);
        }

        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_moves_for_piece_count(SimplechessGame game, const SimplechessSquare* square, size_t* count) {
    if (!game || !square || !count) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        auto cpp_square = c_to_cpp_square(*square);
        auto moves = g->availableMovesForPiece(cpp_square);
        *count = moves.size();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_moves_for_piece(SimplechessGame game, const SimplechessSquare* square, SimplechessPieceMove* moves, size_t moves_size) {
    if (!game || !square || !moves) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        auto cpp_square = c_to_cpp_square(*square);
        auto cpp_moves = g->availableMovesForPiece(cpp_square);

        if (moves_size < cpp_moves.size()) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }

        size_t i = 0;
        for (const auto& move : cpp_moves) {
            moves[i++] = cpp_to_c_piece_move(move);
        }

        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_square_from_rank_and_file(uint8_t rank, char file, SimplechessSquare* square) {
    if (!square) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_square = simplechess::Square::fromRankAndFile(rank, file);
        *square = cpp_to_c_square(cpp_square);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_square_from_string(const char* algebraic, SimplechessSquare* square) {
    if (!algebraic || !square) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_square = simplechess::Square::fromString(std::string(algebraic));
        *square = cpp_to_c_square(cpp_square);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_square_to_string(const SimplechessSquare* square, char* buffer, size_t buffer_size) {
    if (!square || !buffer || buffer_size < 3) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_square = c_to_cpp_square(*square);
        auto str = cpp_square.toString();
        if (str.length() + 1 > buffer_size) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }
        std::strcpy(buffer, str.c_str());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_piece_move_regular(const SimplechessPiece* piece, const SimplechessSquare* src, const SimplechessSquare* dst, SimplechessPieceMove* move) {
    if (!piece || !src || !dst || !move) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_piece = c_to_cpp_piece(*piece);
        auto cpp_src = c_to_cpp_square(*src);
        auto cpp_dst = c_to_cpp_square(*dst);
        auto cpp_move = simplechess::PieceMove::regularMove(cpp_piece, cpp_src, cpp_dst);
        *move = cpp_to_c_piece_move(cpp_move);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_piece_move_promotion(const SimplechessPiece* piece, const SimplechessSquare* src, const SimplechessSquare* dst, SimplechessPieceType promoted_type, SimplechessPieceMove* move) {
    if (!piece || !src || !dst || !move) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    // Validate that piece is a pawn
    if (piece->type != SIMPLECHESS_PIECE_TYPE_PAWN) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    // Validate that promotion type is valid (rook, knight, bishop, or queen)
    if (promoted_type != SIMPLECHESS_PIECE_TYPE_ROOK &&
        promoted_type != SIMPLECHESS_PIECE_TYPE_KNIGHT &&
        promoted_type != SIMPLECHESS_PIECE_TYPE_BISHOP &&
        promoted_type != SIMPLECHESS_PIECE_TYPE_QUEEN) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_piece = c_to_cpp_piece(*piece);
        auto cpp_src = c_to_cpp_square(*src);
        auto cpp_dst = c_to_cpp_square(*dst);
        auto cpp_promoted_type = c_to_cpp_piece_type(promoted_type);
        auto cpp_move = simplechess::PieceMove::pawnPromotion(cpp_piece, cpp_src, cpp_dst, cpp_promoted_type);
        *move = cpp_to_c_piece_move(cpp_move);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

void simplechess_game_destroy(SimplechessGame game) {
    if (game) {
        delete static_cast<simplechess::Game*>(game);
    }
}

// ============================================================================
// Game History Functions
// ============================================================================

SimplechessResult simplechess_game_get_history_length(SimplechessGame game, size_t* length) {
    if (!game || !length) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *length = g->history().size();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_stage_at(SimplechessGame game, size_t index, SimplechessGameStage* stage) {
    if (!game || !stage) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        const auto& history = g->history();
        if (index >= history.size()) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }
        *stage = new simplechess::GameStage(history[index]);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_current_stage(SimplechessGame game, SimplechessGameStage* stage) {
    if (!game || !stage) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *stage = new simplechess::GameStage(g->currentStage());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

// ============================================================================
// Game Stage Functions
// ============================================================================

SimplechessResult simplechess_stage_get_board(SimplechessGameStage stage, SimplechessBoard* board) {
    if (!stage || !board) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        *board = new simplechess::Board(s->board());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_stage_get_move(SimplechessGameStage stage, SimplechessPlayedMove* move, bool* has_move) {
    if (!stage || !move || !has_move) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        const auto& played_move = s->move();
        *has_move = played_move.has_value();
        if (*has_move) {
            *move = new simplechess::PlayedMove(played_move.value());
        } else {
            *move = nullptr;
        }
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_stage_get_active_color(SimplechessGameStage stage, SimplechessColor* color) {
    if (!stage || !color) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        *color = cpp_to_c_color(s->activeColor());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_stage_get_castling_rights(SimplechessGameStage stage, uint8_t* rights) {
    if (!stage || !rights) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        *rights = s->castlingRights();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_stage_get_halfmove_clock(SimplechessGameStage stage, uint16_t* halfmoves) {
    if (!stage || !halfmoves) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        *halfmoves = s->halfMovesSinceLastCaptureOrPawnAdvance();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_stage_get_fullmove_counter(SimplechessGameStage stage, uint16_t* fullmoves) {
    if (!stage || !fullmoves) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        *fullmoves = s->fullMoveCounter();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_stage_get_fen(SimplechessGameStage stage, char* buffer, size_t buffer_size) {
    if (!stage || !buffer) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* s = static_cast<simplechess::GameStage*>(stage);
        const std::string& fen = s->fen();
        if (fen.length() + 1 > buffer_size) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }
        std::strcpy(buffer, fen.c_str());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

// ============================================================================
// Played Move Functions
// ============================================================================

SimplechessResult simplechess_played_move_get_algebraic_notation(SimplechessPlayedMove move, char* buffer, size_t buffer_size) {
    if (!move || !buffer) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* m = static_cast<simplechess::PlayedMove*>(move);
        const std::string& notation = m->inAlgebraicNotation();
        if (notation.length() + 1 > buffer_size) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }
        std::strcpy(buffer, notation.c_str());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_played_move_get_piece_move(SimplechessPlayedMove move, SimplechessPieceMove* piece_move) {
    if (!move || !piece_move) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* m = static_cast<simplechess::PlayedMove*>(move);
        *piece_move = cpp_to_c_piece_move(m->pieceMove());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_played_move_get_captured_piece(SimplechessPlayedMove move, SimplechessPiece* piece, bool* has_capture) {
    if (!move || !piece || !has_capture) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* m = static_cast<simplechess::PlayedMove*>(move);
        const auto& captured = m->capturedPiece();
        *has_capture = captured.has_value();
        if (*has_capture) {
            *piece = cpp_to_c_piece(captured.value());
        }
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_played_move_get_check_type(SimplechessPlayedMove move, SimplechessCheckType* check_type) {
    if (!move || !check_type) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* m = static_cast<simplechess::PlayedMove*>(move);
        *check_type = cpp_to_c_check_type(m->checkType());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_played_move_is_draw_offered(SimplechessPlayedMove move, bool* is_offered) {
    if (!move || !is_offered) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* m = static_cast<simplechess::PlayedMove*>(move);
        *is_offered = m->isDrawOffered();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

// ============================================================================
// Board Functions
// ============================================================================

SimplechessResult simplechess_board_get_piece_at(SimplechessBoard board, SimplechessSquare square, SimplechessPiece* piece, bool* has_piece) {
    if (!board || !piece || !has_piece) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* b = static_cast<simplechess::Board*>(board);
        auto cpp_square = c_to_cpp_square(square);
        auto piece_opt = b->pieceAt(cpp_square);
        *has_piece = piece_opt.has_value();
        if (*has_piece) {
            *piece = cpp_to_c_piece(piece_opt.value());
        }
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_board_get_occupied_squares_count(SimplechessBoard board, size_t* count) {
    if (!board || !count) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* b = static_cast<simplechess::Board*>(board);
        *count = b->occupiedSquares().size();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_board_get_occupied_squares(SimplechessBoard board, SimplechessSquareAndPiece* squares, size_t array_size) {
    if (!board || !squares) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* b = static_cast<simplechess::Board*>(board);
        const auto& occupied = b->occupiedSquares();

        if (array_size < occupied.size()) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }

        size_t i = 0;
        for (const auto& pair : occupied) {
            squares[i++] = cpp_to_c_square_and_piece(pair);
        }

        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

// ============================================================================
// Extended Game Query Functions
// ============================================================================

SimplechessResult simplechess_game_get_halfmove_clock(SimplechessGame game, uint16_t* halfmoves) {
    if (!game || !halfmoves) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *halfmoves = g->currentStage().halfMovesSinceLastCaptureOrPawnAdvance();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_fullmove_counter(SimplechessGame game, uint16_t* fullmoves) {
    if (!game || !fullmoves) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *fullmoves = g->currentStage().fullMoveCounter();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_current_fen(SimplechessGame game, char* buffer, size_t buffer_size) {
    if (!game || !buffer) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        const std::string& fen = g->currentStage().fen();
        if (fen.length() + 1 > buffer_size) {
            return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
        }
        std::strcpy(buffer, fen.c_str());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_castling_rights(SimplechessGame game, uint8_t* rights) {
    if (!game || !rights) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *rights = g->currentStage().castlingRights();
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_game_get_current_board(SimplechessGame game, SimplechessBoard* board) {
    if (!game || !board) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto* g = static_cast<simplechess::Game*>(game);
        *board = new simplechess::Board(g->currentStage().board());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

// ============================================================================
// Additional Utility Functions
// ============================================================================

SimplechessResult simplechess_square_is_inside_boundaries(uint8_t rank, char file, bool* is_inside) {
    if (!is_inside) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        *is_inside = simplechess::Square::isInsideBoundaries(rank, file);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_square_get_color(SimplechessSquare square, SimplechessColor* color) {
    if (!color) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_square = c_to_cpp_square(square);
        *color = cpp_to_c_color(cpp_square.color());
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_squares_are_equal(SimplechessSquare a, SimplechessSquare b, bool* are_equal) {
    if (!are_equal) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_a = c_to_cpp_square(a);
        auto cpp_b = c_to_cpp_square(b);
        *are_equal = (cpp_a == cpp_b);
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

SimplechessResult simplechess_color_get_opposite(SimplechessColor color, SimplechessColor* opposite) {
    if (!opposite) {
        return SIMPLECHESS_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto cpp_color = c_to_cpp_color(color);
        *opposite = cpp_to_c_color(simplechess::oppositeColor(cpp_color));
        return SIMPLECHESS_SUCCESS;
    } catch (...) {
        return handle_exception();
    }
}

// ============================================================================
// Memory Management Functions
// ============================================================================

void simplechess_game_stage_destroy(SimplechessGameStage stage) {
    if (stage) {
        delete static_cast<simplechess::GameStage*>(stage);
    }
}

void simplechess_played_move_destroy(SimplechessPlayedMove move) {
    if (move) {
        delete static_cast<simplechess::PlayedMove*>(move);
    }
}

void simplechess_board_destroy(SimplechessBoard board) {
    if (board) {
        delete static_cast<simplechess::Board*>(board);
    }
}

const char* simplechess_result_to_string(SimplechessResult result) {
    switch (result) {
        case SIMPLECHESS_SUCCESS: return "Success";
        case SIMPLECHESS_ERROR_INVALID_ARGUMENT: return "Invalid argument";
        case SIMPLECHESS_ERROR_ILLEGAL_STATE: return "Illegal state";
        case SIMPLECHESS_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case SIMPLECHESS_ERROR_UNKNOWN: return "Unknown error";
        default: return "Invalid result code";
    }
}

}