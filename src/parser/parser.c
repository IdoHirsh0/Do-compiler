#include <stdlib.h>
#include <stdbool.h>

#include "../global.h"

#include "parser.h"
#include "../general/general.h"
#include "../error_handler/error_handler.h"


void parser_create()
{
    // Create parser
    compiler.parser = (Parser*) calloc(1, sizeof(Parser));
    if (compiler.parser == NULL) exit_memory_error(__FILE__, __LINE__);

    // Create parser's parse table
    parse_table_create();
}

void parser_destroy()
{
    // check for NULL pointer
    if (compiler.parser != NULL)
    {
        // Free parser's parsing table
        parse_table_destroy();

        // Free parser's stack
        parse_stack_destroy();

        // Free the parser
        free(compiler.parser);
        compiler.parser = NULL;
    }
}

void parser_init()
{
    // Initialize parsing table
    parse_table_init();

    // Initialize parser's stack
    // Push the first stack entry onto the stack. For the first entry we only care about its goto_state
    // value because it's the first state we start from. In our case it's state number 0.
    parse_stack_push(parse_stack_init_entry(NULL, 0));

    // Initialize production_rules array according to grammar rules of the language
    parser_init_production_rules();
}

void parser_init_production_rules()
{
    // Holds the number of the current production rule
    int prod_num = 0;
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_PROG, 5 };      // PROG -> prog id : BLOCK :)
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_BLOCK, 2 };     // BLOCK -> STMT BLOCK
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_BLOCK, 1 };     // BLOCK -> done
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_STMT, 1 };      // STMT -> DECL
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_STMT, 1 };      // STMT -> ASSIGN
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_STMT, 1 };      // STMT -> IF_ELSE
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_STMT, 1 };      // STMT -> WHILE
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_DECL, 3 };      // DECL -> data_type id
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_ASSIGN, 5 };    // ASSIGN -> set id = L_LOG_E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_IF_ELSE, 7 };   // IF_ELSE -> if ( L_LOG_E ) : BLOCK ELSE
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_ELSE, 3 };      // ELSE -> else : BLOCK
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_ELSE, 0 };      // ELSE -> epsilon
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_WHILE, 6 };     // WHILE -> while ( L_LOG_E ) : BLOCK
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_L_LOG_E, 3 };   // L_LOG_E -> L_LOG_E l_log_op H_LOG_E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_L_LOG_E, 1 };   // L_LOG_E -> H_LOG_E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_H_LOG_E, 3 };   // H_LOG_E -> H_LOG_E h_log_op BOOL_E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_H_LOG_E, 1 };   // H_LOG_E -> BOOL_E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_BOOL_E, 3 };    // BOOL_E -> BOOL_E bool_op E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_BOOL_E, 1 };    // BOOL_E -> E
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_E, 3 };         // E -> E expr_op T
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_E, 1 };         // E -> T
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_T, 3 };         // T -> T term_op F
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_T, 1 };         // T -> F
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_F, 1 };         // F -> id
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_F, 1 };         // F -> literal
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_F, 3 };         // F -> ( L_LOG_E )
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_F, 2 };         // F -> ! F
    compiler.parser->production_rules[prod_num++] = (Production_Rule) { Non_Terminal_F, 2 };         // F -> - F
}

void parser_shift(Token* token, int goto_state)
{
    // Create a new terminal tree node from the current token
    Parse_Tree_Node* tree_node = parse_tree_init_node(Terminal, token->token_type, token, NULL, 0);
    // Create a new stack entry for created tree node
    Parse_Stack_Entry* stack_entry = parse_stack_init_entry(tree_node, goto_state);
    // Push created stack entry onto the stack
    parse_stack_push(stack_entry);
}

void parser_reduce(int production_rule_num)
{
    // The produced stack entry
    Parse_Stack_Entry* stack_entry;
    // Get the current production rule
    Production_Rule production_rule = compiler.parser->production_rules[production_rule_num];
    // Create an array of the size of number of symbols on the RHS of the production rule that we reduce by
    Parse_Tree_Node** children = (Parse_Tree_Node**) calloc(production_rule.rule_length, sizeof(Parse_Tree_Node*));
    if (children == NULL) exit_memory_error(__FILE__, __LINE__);

    // Pop Length(Production rule) entries from the stack, and put the trees of each entry as a child in the array.
    // From right of array to the left because the production rule is "reversed" in the stack
    for (int i = production_rule.rule_length - 1; i >= 0; i--)
    {
        // Pop entry from the stack
        stack_entry = parse_stack_pop();
        // Place its tree in the children array
        children[i] = stack_entry->tree;
        // Free poped entry
        free(stack_entry);
    }
    // Create a new tree node from the non-terminal on the LHS of the production rule we reduce by
    Parse_Tree_Node* tree_node = parse_tree_init_node(Non_Terminal, production_rule.non_terminal_type, NULL, children, production_rule.rule_length);
    // Create a new stack entry using the created tree node and Goto table
    stack_entry = parse_stack_init_entry(tree_node, compiler.parser->parse_table->goto_table[compiler.parser->parse_stack->goto_state][production_rule.non_terminal_type]);
    // Push created entry onto the stack
    parse_stack_push(stack_entry);
}

Parse_Tree_Node* parser_parse()
{
    // The current token from the source code
    Token* token;
    // The next state to go to
    int state;
    // Current action table cell
    Action action;

    // Input first token from the source code
    token = lexer_get_next_token();

    // While not done parsing. We'll be done parsing by an Accept or Error
    while (true)
    {
        // Get the next state from the top of the stack
        state = compiler.parser->parse_stack->goto_state;

        // Save the current cell in the action table
        action = compiler.parser->parse_table->action_table[state][parse_table_get_terminal_index(token->token_type)];

        // If Action[state, token] == Shift
        if (action.action_type == Action_Shift)
        {
            parser_shift(token, action.state_or_rule);
            // Get next token from the source code
            token = lexer_get_next_token();
        }
        // If Action[state, token] == Reduce
        else if (action.action_type == Action_Reduce)
        {
            parser_reduce(action.state_or_rule);
        }
        // If Action[state, token] == Accept
        else if (action.action_type == Action_Accept)
        {
            // When reached an Accept, the stack only has one entry other than the start entry, which contains the parse tree.
            // Disconect the parse tree from the stack so it won't be destoyred later, and return it.
            Parse_Tree_Node* parse_tree = compiler.parser->parse_stack->tree;
            compiler.parser->parse_stack->tree = NULL;
            return parse_tree;
        }
        // If Action[state, token] == Error
        else
        {
            // If reached an Error, save error line, destroy the parser, output error message and exit
            error_handler_report(compiler.lexer->line, Error_Parser, "Unexpected token %s", token_to_str(token));
        }
    }
}
