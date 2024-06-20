#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	TOKEN_NONE = 0,
	TOKEN_END_OF_FILE,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD_FN,
	TOKEN_KEYWORD_IMPORT,
	TOKEN_LITERAL_STRING,
	TOKEN_SYMBOL_ASTERISK,
	TOKEN_SYMBOL_CURLY_BRACKET_CLOSE,
	TOKEN_SYMBOL_CURLY_BRACKET_OPEN,
	TOKEN_SYMBOL_CURLY_BRACKET_PAIR,
	TOKEN_SYMBOL_COLON_EQUAL,
	TOKEN_SYMBOL_COLON_PAIR,
	TOKEN_SYMBOL_PARENTHESIS_CLOSE,
	TOKEN_SYMBOL_PARENTHESIS_OPEN,
	TOKEN_SYMBOL_PARENTHESIS_PAIR,
	TOKEN_SYMBOL_SEMICOLON,
} Token_Kind;

typedef struct {
	uint64_t id;
	Token_Kind kind;
	const char *source;
	size_t length;
} Token;

typedef struct {
	uint64_t last_id;
	const char *source;
	size_t length;
	size_t index;
} Lexer;

bool are_strings_equal(size_t length, const char *left, const char *right)
{
	size_t i = length;
	while (i-- > 0)
		if (left[i] != right[i])
			return false;
	return true;
}

bool is_lexer_in_bounds(Lexer *lexer)
{
	return (lexer->index < lexer->length &&
		lexer->source[lexer->index] != '\0');
}

bool is_whitespace(char c)
{
	return isspace(c);
}

bool is_lexer_at_name(Lexer *lexer, bool begin_of)
{
	char at = lexer->source[lexer->index];
	if (begin_of)
		return isalpha(at) || at == '_';
	else
		return isalnum(at) || at == '_';
}

bool is_lexer_at_string_begin(Lexer *lexer)
{
	char at = lexer->source[lexer->index];	
	return at == '"';
}

bool is_lexer_at_string_end(Lexer *lexer)
{
	char at = lexer->source[lexer->index];	
	return at == '"' || at == '\n' || at == '\0';
}

Token lexer_next_token(Lexer *lexer)
{
	char curr = lexer->source[lexer->index];
	while (is_lexer_in_bounds(lexer) && is_whitespace(curr))
		curr = lexer->source[++lexer->index];

	Token token = {
		.id = ++lexer->last_id,
	};
	
	if (!is_lexer_in_bounds(lexer)) {
		token.kind = TOKEN_END_OF_FILE;
		return token;
	}
	token.source = &lexer->source[lexer->index];
	
	if (is_lexer_at_name(lexer, true)) {
		token.kind = TOKEN_IDENTIFIER;
		do {
			lexer->index++;
			token.length++;
		} while (is_lexer_in_bounds(lexer) &&
			 is_lexer_at_name(lexer, false));
		switch (token.length) {
		case 2:
			if (are_strings_equal(2, token.source, "fn"))
				token.kind = TOKEN_KEYWORD_FN;
			break;
		case 6:
			if (are_strings_equal(6, token.source, "import"))
				token.kind = TOKEN_KEYWORD_IMPORT;
			break;
		}
		return token;
	}

	if (is_lexer_at_string_begin(lexer)) {
		token.kind = TOKEN_LITERAL_STRING;
		token.source += 1;
		do {
			lexer->index++;
			token.length++;
		} while (is_lexer_in_bounds(lexer) &&
			 !is_lexer_at_string_end(lexer));
		token.length -= 2;
		lexer->index++;
		return token;
	}

	switch (lexer->source[lexer->index]) {
	case ':': {
		lexer->index++;
		switch (lexer->source[lexer->index]) {
		case ':':
			token.kind = TOKEN_SYMBOL_COLON_PAIR;
			token.length = 2;
			lexer->index++;
			return token;
		case '=':
			token.kind = TOKEN_SYMBOL_COLON_EQUAL;
			token.length = 2;
			lexer->index++;
			return token;
		}
	} break;
	case '*':
		token.kind = TOKEN_SYMBOL_ASTERISK;
		token.length = 1;
		lexer->index++;
		return token;
		break;
	case ';':
		token.kind = TOKEN_SYMBOL_SEMICOLON;
		token.length = 1;
		lexer->index++;
		return token;
		break;
	case '(':
		lexer->index++;
		switch (lexer->source[lexer->index]) {
		case ')':
			token.kind = TOKEN_SYMBOL_PARENTHESIS_PAIR;
			token.length = 2;
			lexer->index++;
			return token;
		default:
			token.kind = TOKEN_SYMBOL_PARENTHESIS_OPEN;
			token.length = 1;
			return token;
		}
		break;
	case ')':
		token.kind = TOKEN_SYMBOL_PARENTHESIS_CLOSE;
		token.length = 1;
		lexer->index++;
		return token;
		break;
	case '{':
		lexer->index++;
		switch (lexer->source[lexer->index]) {
		case '}':
			token.kind = TOKEN_SYMBOL_CURLY_BRACKET_CLOSE;
			token.length++;
			lexer->index++;
			return token;
		default:
			token.kind = TOKEN_SYMBOL_CURLY_BRACKET_OPEN;
			token.length = 1;
			return token;
		}
		break;
	case '}':
		token.kind = TOKEN_SYMBOL_CURLY_BRACKET_CLOSE;
		token.source = &lexer->source[lexer->index];
		token.length = 1;
		lexer->index++;
		return token;
		break;
	}
	
	lexer->index++;
	token.kind = TOKEN_NONE;
	return token;
}

const char *name_of_token_kind(Token_Kind kind)
{
	switch (kind) {
	case TOKEN_NONE: return "Token_Kind::NONE";
	case TOKEN_END_OF_FILE: return "Token_Kind::END_OF_FILE";
	case TOKEN_IDENTIFIER: return "Token_Kind::IDENTIFIER";
	case TOKEN_KEYWORD_FN: return "Token_Kind::Keyword::FN";
	case TOKEN_KEYWORD_IMPORT: return "Token_Kind::Keyword::IMPORT";
	case TOKEN_LITERAL_STRING: return "Token_Kind::Literal::STRING";
	case TOKEN_SYMBOL_ASTERISK: return "Token_Kind::Symbol::ASTERISK";
	case TOKEN_SYMBOL_CURLY_BRACKET_CLOSE: return "Token_Kind::Symbol::CURLY_BRACKET('CLOSED)";
	case TOKEN_SYMBOL_CURLY_BRACKET_OPEN: return "Token_Kind::Symbol::CURLY_BRACKET('OPEN)";
	case TOKEN_SYMBOL_CURLY_BRACKET_PAIR: return "Token_Kind::Symbol::CURLY_BRACKET('PAIR)";
	case TOKEN_SYMBOL_COLON_EQUAL: return "Token_Kind::Symbol::COLON_EQUAL";
	case TOKEN_SYMBOL_COLON_PAIR: return "Token_Kind::Symbol::COLON_PAIR";
	case TOKEN_SYMBOL_PARENTHESIS_CLOSE: return "Token_Kind::Symbol::PARENTHESIS('CLOSED)";
	case TOKEN_SYMBOL_PARENTHESIS_OPEN: return "Token_Kind::Symbol::PARENTHESIS('OPEN)";
	case TOKEN_SYMBOL_PARENTHESIS_PAIR: return "Token_Kind::Symbol::PARENTHESIS('PAIR)";
	case TOKEN_SYMBOL_SEMICOLON: return "Token_Kind::Symbol::SEMICOLON";
	default: return "Not a Token_Kind";
	}
}

void token_to_string(const Token *token, char *buffer, size_t bytes)
{
	const char *kind_name = name_of_token_kind(token->kind);
	switch (token->kind) {
	case TOKEN_IDENTIFIER:
	case TOKEN_LITERAL_STRING:
		snprintf(buffer, bytes, "%s: #%zu, \"%.*s\", x%zu",
			 kind_name, token->id, (int)token->length, token->source, token->length);
		break;
	default:
		snprintf(buffer, bytes, "%s: #%zu", kind_name, token->id);
		break;
	}
}

char *read_entire_file(const char *path, size_t *out_length)
{
	FILE *file = fopen(path, "rb");
	if (!file)
		return NULL;
	long length;
	char *buffer;
	if (fseek(file, 0, SEEK_END) < 0 ||
	    (length = ftell(file)) < 0 ||
	    fseek(file, 0, SEEK_SET) < 0 ||
	    !(buffer = malloc(sizeof(char) * length))) {
		fclose(file);
		return NULL;
	}
	if (fread(buffer, sizeof(char), length, file) != length) {
		free(buffer);
		fclose(file);
		return NULL;
	}
	fclose(file);
	if (out_length)
		*out_length = (size_t)length;
	return buffer;
}

const char *ext_of_file(const char *path)
{
	const char *p = path, *x = NULL;
	while (*p != '\0') {
		if (*p == '.')
			x = p;
		p += 1;
	}
	return x ? x + 1 : NULL;
}

bool is_src_file(const char *path)
{
	const char *ext = ext_of_file(path);
	return strcmp(ext, "knv") == 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		const char *program = argv[0];
		fprintf(stderr, "ERROR: Input not provided.\n");
		fprintf(stderr, "USAGE: %s args\n", program);
		return 1;
	}

	const char *input_path = argv[1];
	if (!is_src_file(input_path)) {
		fprintf(stderr, "ERROR: %s is not a source file.\n", input_path);
		return 1;
	}

	size_t input_length;
	char *input_data = read_entire_file(input_path, &input_length);
	if (!input_data) {
		fprintf(stderr, "ERROR: Could not read file %s.\n", input_path);
		return 1;
	}

	Lexer lexer = {
		.last_id = 0,
		.source = input_data,
		.length = input_length,
		.index = 0,
	};
	
	Token token;
	do {
		token = lexer_next_token(&lexer);
		char buffer[512];
		token_to_string(&token, buffer, sizeof(buffer));
		printf("%s\n", buffer);
	} while (token.kind != TOKEN_END_OF_FILE);
	
	return 0;
}
