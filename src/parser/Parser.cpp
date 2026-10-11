// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "Parser.hpp"


// Unqualified %code blocks.
#line 31 "src/parser/Parser.yy"

#include "ParserDriver.hpp"

yy::Parser::symbol_type yylex(ParserDriver& driver);

void yy::Parser::error(const Location& loc, const std::string& msg) {
    driver.error(loc, msg);
}

#line 56 "src/parser/Parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 6 "src/parser/Parser.yy"
namespace yy {
#line 149 "src/parser/Parser.cpp"

  /// Build a parser object.
  Parser::Parser (ParserDriver& driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.YY_MOVE_OR_COPY< AstNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.YY_MOVE_OR_COPY< Declaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.YY_MOVE_OR_COPY< Expression* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter: // parameter
        value.YY_MOVE_OR_COPY< ParameterDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.YY_MOVE_OR_COPY< RoutineDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.YY_MOVE_OR_COPY< Statement* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.YY_MOVE_OR_COPY< TypeDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.YY_MOVE_OR_COPY< TypeNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_variable_declaration: // variable_declaration
        value.YY_MOVE_OR_COPY< VariableDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optional_reverse: // optional_reverse
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_REAL_LITERAL: // REAL_LITERAL
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INT_LITERAL: // INT_LITERAL
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_field_name: // field_name
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.YY_MOVE_OR_COPY< std::vector<AstNode*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.YY_MOVE_OR_COPY< std::vector<Expression*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.YY_MOVE_OR_COPY< std::vector<ParameterDeclaration*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.YY_MOVE_OR_COPY< std::vector<VariableDeclaration*> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.move< AstNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< Declaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.move< Expression* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter: // parameter
        value.move< ParameterDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.move< RoutineDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.move< Statement* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.move< TypeDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.move< TypeNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_variable_declaration: // variable_declaration
        value.move< VariableDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optional_reverse: // optional_reverse
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_REAL_LITERAL: // REAL_LITERAL
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INT_LITERAL: // INT_LITERAL
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_field_name: // field_name
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.move< std::vector<AstNode*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.move< std::vector<Expression*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.move< std::vector<ParameterDeclaration*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.move< std::vector<VariableDeclaration*> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.copy< AstNode* > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.copy< Declaration* > (that.value);
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.copy< Expression* > (that.value);
        break;

      case symbol_kind::S_parameter: // parameter
        value.copy< ParameterDeclaration* > (that.value);
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.copy< RoutineDeclaration* > (that.value);
        break;

      case symbol_kind::S_statement: // statement
        value.copy< Statement* > (that.value);
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.copy< TypeDeclaration* > (that.value);
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.copy< TypeNode* > (that.value);
        break;

      case symbol_kind::S_variable_declaration: // variable_declaration
        value.copy< VariableDeclaration* > (that.value);
        break;

      case symbol_kind::S_optional_reverse: // optional_reverse
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_REAL_LITERAL: // REAL_LITERAL
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_INT_LITERAL: // INT_LITERAL
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_field_name: // field_name
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.copy< std::vector<AstNode*> > (that.value);
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.copy< std::vector<Expression*> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.copy< std::vector<ParameterDeclaration*> > (that.value);
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.copy< std::vector<VariableDeclaration*> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.move< AstNode* > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< Declaration* > (that.value);
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.move< Expression* > (that.value);
        break;

      case symbol_kind::S_parameter: // parameter
        value.move< ParameterDeclaration* > (that.value);
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.move< RoutineDeclaration* > (that.value);
        break;

      case symbol_kind::S_statement: // statement
        value.move< Statement* > (that.value);
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.move< TypeDeclaration* > (that.value);
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.move< TypeNode* > (that.value);
        break;

      case symbol_kind::S_variable_declaration: // variable_declaration
        value.move< VariableDeclaration* > (that.value);
        break;

      case symbol_kind::S_optional_reverse: // optional_reverse
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_REAL_LITERAL: // REAL_LITERAL
        value.move< double > (that.value);
        break;

      case symbol_kind::S_INT_LITERAL: // INT_LITERAL
        value.move< int > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_field_name: // field_name
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.move< std::vector<AstNode*> > (that.value);
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.move< std::vector<Expression*> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.move< std::vector<ParameterDeclaration*> > (that.value);
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.move< std::vector<VariableDeclaration*> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (driver));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        yylhs.value.emplace< AstNode* > ();
        break;

      case symbol_kind::S_declaration: // declaration
        yylhs.value.emplace< Declaration* > ();
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        yylhs.value.emplace< Expression* > ();
        break;

      case symbol_kind::S_parameter: // parameter
        yylhs.value.emplace< ParameterDeclaration* > ();
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        yylhs.value.emplace< RoutineDeclaration* > ();
        break;

      case symbol_kind::S_statement: // statement
        yylhs.value.emplace< Statement* > ();
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        yylhs.value.emplace< TypeDeclaration* > ();
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        yylhs.value.emplace< TypeNode* > ();
        break;

      case symbol_kind::S_variable_declaration: // variable_declaration
        yylhs.value.emplace< VariableDeclaration* > ();
        break;

      case symbol_kind::S_optional_reverse: // optional_reverse
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_REAL_LITERAL: // REAL_LITERAL
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_INT_LITERAL: // INT_LITERAL
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_field_name: // field_name
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        yylhs.value.emplace< std::vector<AstNode*> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        yylhs.value.emplace< std::vector<Expression*> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        yylhs.value.emplace< std::vector<ParameterDeclaration*> > ();
        break;

      case symbol_kind::S_record_fields: // record_fields
        yylhs.value.emplace< std::vector<VariableDeclaration*> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: opt_seps top_items opt_seps
#line 87 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> decls;
        for (auto* item : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            decls.emplace_back(item);
        }
        driver.set_ast_root(std::make_unique<Program>(std::move(decls), yystack_[2].location));
    }
#line 944 "src/parser/Parser.cpp"
    break;

  case 3: // program: opt_seps
#line 95 "src/parser/Parser.yy"
    {
        driver.set_ast_root(std::make_unique<Program>(std::vector<std::unique_ptr<AstNode>>{}, yystack_[0].location));
    }
#line 952 "src/parser/Parser.cpp"
    break;

  case 4: // top_items: top_item
#line 102 "src/parser/Parser.yy"
    {
        std::vector<AstNode*> items;
        items.push_back(yystack_[0].value.as < AstNode* > ());
        yylhs.value.as < std::vector<AstNode*> > () = std::move(items);
    }
#line 962 "src/parser/Parser.cpp"
    break;

  case 5: // top_items: top_items seps top_item
#line 108 "src/parser/Parser.yy"
    {
        yystack_[2].value.as < std::vector<AstNode*> > ().push_back(yystack_[0].value.as < AstNode* > ());
        yylhs.value.as < std::vector<AstNode*> > () = std::move(yystack_[2].value.as < std::vector<AstNode*> > ());
    }
#line 971 "src/parser/Parser.cpp"
    break;

  case 6: // top_item: routine_declaration
#line 116 "src/parser/Parser.yy"
    {
        yylhs.value.as < AstNode* > () = yystack_[0].value.as < RoutineDeclaration* > ();
    }
#line 979 "src/parser/Parser.cpp"
    break;

  case 7: // top_item: declaration
#line 120 "src/parser/Parser.yy"
    {
        yylhs.value.as < AstNode* > () = yystack_[0].value.as < Declaration* > ();
    }
#line 987 "src/parser/Parser.cpp"
    break;

  case 8: // routine_declaration: ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type IS body END
#line 127 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<ParameterDeclaration>> param_vec;
        for (auto* p : yystack_[5].value.as < std::vector<ParameterDeclaration*> > ()) {
            param_vec.emplace_back(p);
        }
        std::vector<std::unique_ptr<AstNode>> body_vec;
        for (auto* item : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            body_vec.emplace_back(item);
        }
        yylhs.value.as < RoutineDeclaration* > () = new RoutineDeclaration(yystack_[7].value.as < std::string > (), std::move(param_vec), std::unique_ptr<TypeNode>(yystack_[3].value.as < TypeNode* > ()), std::move(body_vec), yystack_[8].location);
    }
#line 1003 "src/parser/Parser.cpp"
    break;

  case 9: // routine_declaration: ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type ARROW expression
#line 139 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<ParameterDeclaration>> param_vec;
        for (auto* p : yystack_[4].value.as < std::vector<ParameterDeclaration*> > ()) {
            param_vec.emplace_back(p);
        }
        yylhs.value.as < RoutineDeclaration* > () = new RoutineDeclaration(yystack_[6].value.as < std::string > (), std::move(param_vec), std::unique_ptr<TypeNode>(yystack_[2].value.as < TypeNode* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[7].location);
    }
#line 1015 "src/parser/Parser.cpp"
    break;

  case 10: // routine_declaration: ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type
#line 147 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<ParameterDeclaration>> param_vec;
        for (auto* p : yystack_[2].value.as < std::vector<ParameterDeclaration*> > ()) {
            param_vec.emplace_back(p);
        }
        yylhs.value.as < RoutineDeclaration* > () = new RoutineDeclaration(yystack_[4].value.as < std::string > (), std::move(param_vec), std::unique_ptr<TypeNode>(yystack_[0].value.as < TypeNode* > ()), yystack_[5].location);
    }
#line 1027 "src/parser/Parser.cpp"
    break;

  case 11: // parameter_list: %empty
#line 158 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<ParameterDeclaration*> > () = std::vector<ParameterDeclaration*>{};
    }
#line 1035 "src/parser/Parser.cpp"
    break;

  case 12: // parameter_list: parameters
#line 162 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<ParameterDeclaration*> > () = std::move(yystack_[0].value.as < std::vector<ParameterDeclaration*> > ());
    }
#line 1043 "src/parser/Parser.cpp"
    break;

  case 13: // parameters: parameter
#line 169 "src/parser/Parser.yy"
    {
        std::vector<ParameterDeclaration*> params;
        params.push_back(yystack_[0].value.as < ParameterDeclaration* > ());
        yylhs.value.as < std::vector<ParameterDeclaration*> > () = std::move(params);
    }
#line 1053 "src/parser/Parser.cpp"
    break;

  case 14: // parameters: parameters COMMA parameter
#line 175 "src/parser/Parser.yy"
    {
        yystack_[2].value.as < std::vector<ParameterDeclaration*> > ().push_back(yystack_[0].value.as < ParameterDeclaration* > ());
        yylhs.value.as < std::vector<ParameterDeclaration*> > () = std::move(yystack_[2].value.as < std::vector<ParameterDeclaration*> > ());
    }
#line 1062 "src/parser/Parser.cpp"
    break;

  case 15: // parameter: IDENTIFIER COLON type_spec
#line 183 "src/parser/Parser.yy"
    {
        yylhs.value.as < ParameterDeclaration* > () = new ParameterDeclaration(yystack_[2].value.as < std::string > (), std::unique_ptr<TypeNode>(yystack_[0].value.as < TypeNode* > ()), yystack_[2].location);
    }
#line 1070 "src/parser/Parser.cpp"
    break;

  case 16: // parameter: IDENTIFIER
#line 187 "src/parser/Parser.yy"
    {
        yylhs.value.as < ParameterDeclaration* > () = new ParameterDeclaration(yystack_[0].value.as < std::string > (), yystack_[0].location);
    }
#line 1078 "src/parser/Parser.cpp"
    break;

  case 17: // optional_return_type: %empty
#line 194 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = nullptr;
    }
#line 1086 "src/parser/Parser.cpp"
    break;

  case 18: // optional_return_type: COLON type_spec
#line 198 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = yystack_[0].value.as < TypeNode* > ();
    }
#line 1094 "src/parser/Parser.cpp"
    break;

  case 19: // declaration: variable_declaration
#line 205 "src/parser/Parser.yy"
    {
        yylhs.value.as < Declaration* > () = yystack_[0].value.as < VariableDeclaration* > ();
    }
#line 1102 "src/parser/Parser.cpp"
    break;

  case 20: // declaration: type_declaration
#line 209 "src/parser/Parser.yy"
    {
        yylhs.value.as < Declaration* > () = yystack_[0].value.as < TypeDeclaration* > ();
    }
#line 1110 "src/parser/Parser.cpp"
    break;

  case 21: // type_declaration: TYPE IDENTIFIER IS type_spec
#line 216 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeDeclaration* > () = new TypeDeclaration(yystack_[2].value.as < std::string > (), std::unique_ptr<TypeNode>(yystack_[0].value.as < TypeNode* > ()), yystack_[3].location);
    }
#line 1118 "src/parser/Parser.cpp"
    break;

  case 22: // type_spec: primitive_type
#line 223 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = yystack_[0].value.as < TypeNode* > ();
    }
#line 1126 "src/parser/Parser.cpp"
    break;

  case 23: // type_spec: IDENTIFIER
#line 227 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = new IdentifierTypeNode(yystack_[0].value.as < std::string > (), yystack_[0].location);
    }
#line 1134 "src/parser/Parser.cpp"
    break;

  case 24: // type_spec: array_type
#line 231 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = yystack_[0].value.as < TypeNode* > ();
    }
#line 1142 "src/parser/Parser.cpp"
    break;

  case 25: // type_spec: record_type
#line 235 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = yystack_[0].value.as < TypeNode* > ();
    }
#line 1150 "src/parser/Parser.cpp"
    break;

  case 26: // primitive_type: KW_INTEGER
#line 241 "src/parser/Parser.yy"
               { yylhs.value.as < TypeNode* > () = new PrimitiveTypeNode("integer", yystack_[0].location); }
#line 1156 "src/parser/Parser.cpp"
    break;

  case 27: // primitive_type: KW_REAL
#line 242 "src/parser/Parser.yy"
               { yylhs.value.as < TypeNode* > () = new PrimitiveTypeNode("real", yystack_[0].location); }
#line 1162 "src/parser/Parser.cpp"
    break;

  case 28: // primitive_type: KW_BOOLEAN
#line 243 "src/parser/Parser.yy"
               { yylhs.value.as < TypeNode* > () = new PrimitiveTypeNode("boolean", yystack_[0].location); }
#line 1168 "src/parser/Parser.cpp"
    break;

  case 29: // array_type: ARRAY LBRACKET expression RBRACKET type_spec
#line 248 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = new ArrayTypeNode(std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<TypeNode>(yystack_[0].value.as < TypeNode* > ()), yystack_[4].location);
    }
#line 1176 "src/parser/Parser.cpp"
    break;

  case 30: // array_type: ARRAY LBRACKET RBRACKET type_spec
#line 252 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = new ArrayTypeNode(nullptr, std::unique_ptr<TypeNode>(yystack_[0].value.as < TypeNode* > ()), yystack_[3].location);
    }
#line 1184 "src/parser/Parser.cpp"
    break;

  case 31: // record_type: RECORD opt_seps record_fields opt_seps END
#line 259 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<VariableDeclaration>> decls;
        for (auto* d : yystack_[2].value.as < std::vector<VariableDeclaration*> > ()) {
            decls.emplace_back(d);
        }
        yylhs.value.as < TypeNode* > () = new RecordTypeNode(std::move(decls), yystack_[4].location);
    }
#line 1196 "src/parser/Parser.cpp"
    break;

  case 32: // record_type: RECORD opt_seps END
#line 267 "src/parser/Parser.yy"
    {
        yylhs.value.as < TypeNode* > () = new RecordTypeNode({}, yystack_[2].location);
    }
#line 1204 "src/parser/Parser.cpp"
    break;

  case 33: // record_fields: variable_declaration
#line 274 "src/parser/Parser.yy"
    {
        std::vector<VariableDeclaration*> fields;
        fields.push_back(yystack_[0].value.as < VariableDeclaration* > ());
        yylhs.value.as < std::vector<VariableDeclaration*> > () = std::move(fields);
    }
#line 1214 "src/parser/Parser.cpp"
    break;

  case 34: // record_fields: record_fields seps variable_declaration
#line 280 "src/parser/Parser.yy"
    {
        yystack_[2].value.as < std::vector<VariableDeclaration*> > ().push_back(yystack_[0].value.as < VariableDeclaration* > ());
        yylhs.value.as < std::vector<VariableDeclaration*> > () = std::move(yystack_[2].value.as < std::vector<VariableDeclaration*> > ());
    }
#line 1223 "src/parser/Parser.cpp"
    break;

  case 35: // variable_declaration: VAR IDENTIFIER COLON type_spec IS expression
#line 288 "src/parser/Parser.yy"
    {
        yylhs.value.as < VariableDeclaration* > () = new VariableDeclaration(yystack_[4].value.as < std::string > (), std::unique_ptr<TypeNode>(yystack_[2].value.as < TypeNode* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[5].location);
    }
#line 1231 "src/parser/Parser.cpp"
    break;

  case 36: // variable_declaration: VAR IDENTIFIER COLON type_spec
#line 292 "src/parser/Parser.yy"
    {
        yylhs.value.as < VariableDeclaration* > () = new VariableDeclaration(yystack_[2].value.as < std::string > (), std::unique_ptr<TypeNode>(yystack_[0].value.as < TypeNode* > ()), nullptr, yystack_[3].location);
    }
#line 1239 "src/parser/Parser.cpp"
    break;

  case 37: // variable_declaration: VAR IDENTIFIER IS expression
#line 296 "src/parser/Parser.yy"
    {
        yylhs.value.as < VariableDeclaration* > () = new VariableDeclaration(yystack_[2].value.as < std::string > (), nullptr, std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[3].location);
    }
#line 1247 "src/parser/Parser.cpp"
    break;

  case 38: // body: opt_seps
#line 303 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<AstNode*> > () = std::vector<AstNode*>{};
    }
#line 1255 "src/parser/Parser.cpp"
    break;

  case 39: // body: opt_seps body_items opt_seps
#line 307 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<AstNode*> > () = std::move(yystack_[1].value.as < std::vector<AstNode*> > ());
    }
#line 1263 "src/parser/Parser.cpp"
    break;

  case 40: // body_items: body_item
#line 314 "src/parser/Parser.yy"
    {
        std::vector<AstNode*> items;
        items.push_back(yystack_[0].value.as < AstNode* > ());
        yylhs.value.as < std::vector<AstNode*> > () = std::move(items);
    }
#line 1273 "src/parser/Parser.cpp"
    break;

  case 41: // body_items: body_items seps body_item
#line 320 "src/parser/Parser.yy"
    {
        yystack_[2].value.as < std::vector<AstNode*> > ().push_back(yystack_[0].value.as < AstNode* > ());
        yylhs.value.as < std::vector<AstNode*> > () = std::move(yystack_[2].value.as < std::vector<AstNode*> > ());
    }
#line 1282 "src/parser/Parser.cpp"
    break;

  case 42: // body_item: statement
#line 328 "src/parser/Parser.yy"
    {
        yylhs.value.as < AstNode* > () = yystack_[0].value.as < Statement* > ();
    }
#line 1290 "src/parser/Parser.cpp"
    break;

  case 43: // body_item: declaration
#line 332 "src/parser/Parser.yy"
    {
        yylhs.value.as < AstNode* > () = yystack_[0].value.as < Declaration* > ();
    }
#line 1298 "src/parser/Parser.cpp"
    break;

  case 44: // statement: modifiable_primary ASSIGN expression
#line 339 "src/parser/Parser.yy"
    {
        yylhs.value.as < Statement* > () = new AssignStatement(std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1306 "src/parser/Parser.cpp"
    break;

  case 45: // statement: IDENTIFIER LPAREN argument_list RPAREN
#line 343 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<Expression>> args;
        for (auto* arg : yystack_[1].value.as < std::vector<Expression*> > ()) {
            args.emplace_back(arg);
        }
        yylhs.value.as < Statement* > () = new RoutineCallStmt(yystack_[3].value.as < std::string > (), std::move(args), yystack_[3].location);
    }
#line 1318 "src/parser/Parser.cpp"
    break;

  case 46: // statement: PRINT expression_list
#line 351 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<Expression>> exprs;
        for (auto* e : yystack_[0].value.as < std::vector<Expression*> > ()) {
            exprs.emplace_back(e);
        }
        yylhs.value.as < Statement* > () = new PrintStatement(std::move(exprs), yystack_[1].location);
    }
#line 1330 "src/parser/Parser.cpp"
    break;

  case 47: // statement: IF expression THEN body END
#line 359 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> then_body;
        for (auto* n : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            then_body.emplace_back(n);
        }
        yylhs.value.as < Statement* > () = new IfStatement(std::unique_ptr<Expression>(yystack_[3].value.as < Expression* > ()), std::move(then_body), std::vector<std::unique_ptr<AstNode>>{}, yystack_[4].location);
    }
#line 1342 "src/parser/Parser.cpp"
    break;

  case 48: // statement: IF expression THEN body ELSE body END
#line 367 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> then_body;
        for (auto* n : yystack_[3].value.as < std::vector<AstNode*> > ()) {
            then_body.emplace_back(n);
        }
        std::vector<std::unique_ptr<AstNode>> else_body;
        for (auto* n : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            else_body.emplace_back(n);
        }
        yylhs.value.as < Statement* > () = new IfStatement(std::unique_ptr<Expression>(yystack_[5].value.as < Expression* > ()), std::move(then_body), std::move(else_body), yystack_[6].location);
    }
#line 1358 "src/parser/Parser.cpp"
    break;

  case 49: // statement: WHILE expression LOOP body END
#line 379 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> b;
        for (auto* n : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            b.emplace_back(n);
        }
        yylhs.value.as < Statement* > () = new WhileStatement(std::unique_ptr<Expression>(yystack_[3].value.as < Expression* > ()), std::move(b), yystack_[4].location);
    }
#line 1370 "src/parser/Parser.cpp"
    break;

  case 50: // statement: FOR IDENTIFIER IN expression RANGE expression optional_reverse LOOP body END
#line 387 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> b;
        for (auto* n : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            b.emplace_back(n);
        }
        yylhs.value.as < Statement* > () = new ForStatement(yystack_[8].value.as < std::string > (), std::unique_ptr<Expression>(yystack_[6].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[4].value.as < Expression* > ()), yystack_[3].value.as < bool > (), std::move(b), yystack_[9].location);
    }
#line 1382 "src/parser/Parser.cpp"
    break;

  case 51: // statement: FOR IDENTIFIER IN expression optional_reverse LOOP body END
#line 395 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> b;
        for (auto* n : yystack_[1].value.as < std::vector<AstNode*> > ()) {
            b.emplace_back(n);
        }
        yylhs.value.as < Statement* > () = new ForStatement(yystack_[6].value.as < std::string > (), std::unique_ptr<Expression>(yystack_[4].value.as < Expression* > ()), yystack_[3].value.as < bool > (), std::move(b), yystack_[7].location);
    }
#line 1394 "src/parser/Parser.cpp"
    break;

  case 52: // statement: RETURN
#line 403 "src/parser/Parser.yy"
    {
        yylhs.value.as < Statement* > () = new ReturnStatement(nullptr, yystack_[0].location);
    }
#line 1402 "src/parser/Parser.cpp"
    break;

  case 53: // statement: RETURN expression
#line 407 "src/parser/Parser.yy"
    {
        yylhs.value.as < Statement* > () = new ReturnStatement(std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1410 "src/parser/Parser.cpp"
    break;

  case 54: // optional_reverse: %empty
#line 414 "src/parser/Parser.yy"
    {
        yylhs.value.as < bool > () = false;
    }
#line 1418 "src/parser/Parser.cpp"
    break;

  case 55: // optional_reverse: REVERSE
#line 418 "src/parser/Parser.yy"
    {
        yylhs.value.as < bool > () = true;
    }
#line 1426 "src/parser/Parser.cpp"
    break;

  case 56: // modifiable_primary: IDENTIFIER
#line 425 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new Identifier(yystack_[0].value.as < std::string > (), yystack_[0].location);
    }
#line 1434 "src/parser/Parser.cpp"
    break;

  case 57: // modifiable_primary: modifiable_primary DOT field_name opt_empty_parens
#line 429 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new FieldAccessExpr(std::unique_ptr<Expression>(yystack_[3].value.as < Expression* > ()), yystack_[1].value.as < std::string > (), yystack_[2].location);
    }
#line 1442 "src/parser/Parser.cpp"
    break;

  case 58: // modifiable_primary: modifiable_primary LBRACKET expression RBRACKET
#line 433 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new ElementAccessExpr(std::unique_ptr<Expression>(yystack_[3].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[1].value.as < Expression* > ()), yystack_[2].location);
    }
#line 1450 "src/parser/Parser.cpp"
    break;

  case 61: // field_name: IDENTIFIER
#line 444 "src/parser/Parser.yy"
               { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 1456 "src/parser/Parser.cpp"
    break;

  case 62: // field_name: SIZE
#line 445 "src/parser/Parser.yy"
               { yylhs.value.as < std::string > () = "size"; }
#line 1462 "src/parser/Parser.cpp"
    break;

  case 63: // expression: INT_LITERAL
#line 450 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new IntLiteral(yystack_[0].value.as < int > (), yystack_[0].location);
    }
#line 1470 "src/parser/Parser.cpp"
    break;

  case 64: // expression: REAL_LITERAL
#line 454 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new RealLiteral(yystack_[0].value.as < double > (), yystack_[0].location);
    }
#line 1478 "src/parser/Parser.cpp"
    break;

  case 65: // expression: TRUE
#line 458 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BoolLiteral(true, yystack_[0].location);
    }
#line 1486 "src/parser/Parser.cpp"
    break;

  case 66: // expression: FALSE
#line 462 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BoolLiteral(false, yystack_[0].location);
    }
#line 1494 "src/parser/Parser.cpp"
    break;

  case 67: // expression: modifiable_primary
#line 466 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = yystack_[0].value.as < Expression* > ();
    }
#line 1502 "src/parser/Parser.cpp"
    break;

  case 68: // expression: LPAREN expression RPAREN
#line 470 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = yystack_[1].value.as < Expression* > ();
    }
#line 1510 "src/parser/Parser.cpp"
    break;

  case 69: // expression: expression PLUS expression
#line 474 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Plus, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1518 "src/parser/Parser.cpp"
    break;

  case 70: // expression: expression MINUS expression
#line 478 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Minus, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1526 "src/parser/Parser.cpp"
    break;

  case 71: // expression: expression MUL expression
#line 482 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Mul, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1534 "src/parser/Parser.cpp"
    break;

  case 72: // expression: expression DIV expression
#line 486 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Div, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1542 "src/parser/Parser.cpp"
    break;

  case 73: // expression: expression MOD expression
#line 490 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Mod, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1550 "src/parser/Parser.cpp"
    break;

  case 74: // expression: expression AND expression
#line 494 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::And, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1558 "src/parser/Parser.cpp"
    break;

  case 75: // expression: expression OR expression
#line 498 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Or, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1566 "src/parser/Parser.cpp"
    break;

  case 76: // expression: expression XOR expression
#line 502 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Xor, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1574 "src/parser/Parser.cpp"
    break;

  case 77: // expression: expression LESS expression
#line 506 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Less, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1582 "src/parser/Parser.cpp"
    break;

  case 78: // expression: expression LESS_EQUAL expression
#line 510 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::LessEqual, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1590 "src/parser/Parser.cpp"
    break;

  case 79: // expression: expression GREATER expression
#line 514 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Greater, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1598 "src/parser/Parser.cpp"
    break;

  case 80: // expression: expression GREATER_EQUAL expression
#line 518 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::GreaterEqual, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1606 "src/parser/Parser.cpp"
    break;

  case 81: // expression: expression EQUAL expression
#line 522 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::Equal, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1614 "src/parser/Parser.cpp"
    break;

  case 82: // expression: expression NOT_EQUAL expression
#line 526 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new BinaryExpr(BinOp::NotEqual, std::unique_ptr<Expression>(yystack_[2].value.as < Expression* > ()), std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1622 "src/parser/Parser.cpp"
    break;

  case 83: // expression: PLUS expression
#line 530 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new UnaryExpr(UnaryOp::Plus, std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1630 "src/parser/Parser.cpp"
    break;

  case 84: // expression: MINUS expression
#line 534 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new UnaryExpr(UnaryOp::Minus, std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1638 "src/parser/Parser.cpp"
    break;

  case 85: // expression: NOT expression
#line 538 "src/parser/Parser.yy"
    {
        yylhs.value.as < Expression* > () = new UnaryExpr(UnaryOp::Not, std::unique_ptr<Expression>(yystack_[0].value.as < Expression* > ()), yystack_[1].location);
    }
#line 1646 "src/parser/Parser.cpp"
    break;

  case 86: // expression: IDENTIFIER LPAREN argument_list RPAREN
#line 542 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<Expression>> args;
        for (auto* arg : yystack_[1].value.as < std::vector<Expression*> > ()) {
            args.emplace_back(arg);
        }
        yylhs.value.as < Expression* > () = new RoutineCallExpr(yystack_[3].value.as < std::string > (), std::move(args), yystack_[3].location);
    }
#line 1658 "src/parser/Parser.cpp"
    break;

  case 87: // argument_list: %empty
#line 553 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<Expression*> > () = std::vector<Expression*>{};
    }
#line 1666 "src/parser/Parser.cpp"
    break;

  case 88: // argument_list: expression_list
#line 557 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<Expression*> > () = std::move(yystack_[0].value.as < std::vector<Expression*> > ());
    }
#line 1674 "src/parser/Parser.cpp"
    break;

  case 89: // expression_list: expression
#line 564 "src/parser/Parser.yy"
    {
        std::vector<Expression*> list;
        list.push_back(yystack_[0].value.as < Expression* > ());
        yylhs.value.as < std::vector<Expression*> > () = std::move(list);
    }
#line 1684 "src/parser/Parser.cpp"
    break;

  case 90: // expression_list: expression_list COMMA expression
#line 570 "src/parser/Parser.yy"
    {
        yystack_[2].value.as < std::vector<Expression*> > ().push_back(yystack_[0].value.as < Expression* > ());
        yylhs.value.as < std::vector<Expression*> > () = std::move(yystack_[2].value.as < std::vector<Expression*> > ());
    }
#line 1693 "src/parser/Parser.cpp"
    break;


#line 1697 "src/parser/Parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // Parser::context.
  Parser::context::context (const Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -125;

  const signed char Parser::yytable_ninf_ = -1;

  const short
  Parser::yypact_[] =
  {
      32,  -125,  -125,    78,  -125,    32,    97,  -125,  -125,     7,
      70,    86,    32,  -125,  -125,  -125,  -125,  -125,    18,    -8,
      44,    10,  -125,   116,   101,   101,    89,  -125,    50,  -125,
    -125,  -125,  -125,   116,   116,   116,   116,   -28,   287,  -125,
    -125,  -125,  -125,    49,    32,    91,  -125,  -125,  -125,  -125,
      59,    52,    60,  -125,   116,  -125,  -125,  -125,   252,   116,
       3,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,    27,    73,   116,   101,    62,
      89,   287,    63,    72,  -125,   216,  -125,  -125,    84,   144,
     144,   144,     0,     0,  -125,  -125,  -125,   298,   298,   298,
     298,   298,   298,   101,   234,  -125,    32,  -125,   287,  -125,
     101,    -7,  -125,  -125,   116,  -125,    85,  -125,  -125,   101,
      16,   122,  -125,    32,   116,   287,  -125,  -125,  -125,  -125,
     124,   293,   287,  -125,    90,   116,   135,   116,   116,   116,
    -125,    32,  -125,  -125,    35,   116,   160,   123,   201,    72,
     287,    54,  -125,   116,    93,    32,   116,    32,  -125,   287,
    -125,   127,   131,    -3,  -125,  -125,   116,   128,  -125,    32,
     176,    32,   130,   129,   139,  -125,    32,  -125,   141,  -125
  };

  const signed char
  Parser::yydefact_[] =
  {
      95,    91,    92,     0,    93,    96,     3,     1,    94,     0,
       0,     0,    95,     4,     6,     7,    20,    19,     0,     0,
       0,    96,     2,     0,     0,     0,    11,     5,    56,    63,
      64,    65,    66,     0,     0,     0,     0,    67,    37,    23,
      26,    27,    28,     0,    95,    36,    22,    24,    25,    21,
      16,     0,    12,    13,    87,    85,    83,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,    89,     0,    88,    68,     0,    61,    62,    59,    74,
      75,    76,    69,    70,    71,    72,    73,    77,    78,    79,
      80,    81,    82,     0,     0,    32,    95,    33,    35,    15,
       0,    10,    14,    86,     0,    58,     0,    57,    30,     0,
      96,     0,    18,    95,     0,    90,    60,    29,    34,    31,
       0,    38,     9,     8,    56,     0,     0,     0,     0,    52,
      43,    95,    40,    42,     0,    87,     0,     0,     0,    46,
      53,    96,    39,     0,     0,    95,     0,    95,    41,    44,
      45,     0,    54,     0,    49,    55,     0,     0,    47,    95,
      54,    95,     0,     0,     0,    48,    95,    51,     0,    50
  };

  const short
  Parser::yypgoto_[] =
  {
    -125,  -125,  -125,   133,  -125,  -125,  -125,    75,  -125,  -124,
    -125,   -20,  -125,  -125,  -125,  -125,   -54,   -75,  -125,     5,
    -125,   -13,  -123,  -125,  -125,   -21,    13,    21,    -5,   -11,
      11
  };

  const unsigned char
  Parser::yydefgoto_[] =
  {
       0,     3,    12,    13,    14,    51,    52,    53,   111,    15,
      16,    45,    46,    47,    48,   106,    17,   130,   141,   142,
     143,   167,    37,   117,    88,    81,    82,    83,     4,     5,
     131
  };

  const short
  Parser::yytable_[] =
  {
       8,    21,    38,    25,   123,    49,    86,   140,   144,   168,
      18,     6,    55,    56,    57,    58,     8,   169,     9,    10,
      11,    59,   107,    22,     9,    60,    87,   140,   144,    23,
      28,    29,    30,    31,    32,    66,    67,    68,    85,   124,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   104,    76,   108,   134,   109,    33,
      34,    35,     9,    10,     1,     2,   128,   135,   136,    24,
       1,     2,   137,    19,    36,   138,   139,   103,     7,   153,
     161,     9,   163,   118,    59,   105,     1,     2,    60,    20,
     122,    26,    50,   125,   172,   120,   174,    54,    75,   127,
      79,   178,    77,   132,    39,     9,    10,    11,     1,     2,
      78,   113,    80,   110,   146,     8,   148,   121,   150,    28,
      29,    30,    31,    32,   114,    40,    41,    42,    43,    44,
     151,   116,   159,   126,   129,   162,   133,   145,   147,   164,
     156,   160,   175,   171,   176,   170,     8,   165,    33,    34,
      35,   177,   152,   179,    27,   112,   158,   173,   154,   149,
      61,    62,    63,    36,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,   155,   166,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,     0,    61,
      62,    63,   165,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,     0,    61,    62,    63,     0,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
     157,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,     0,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    61,    62,    63,     0,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
       0,     0,     0,    61,    62,    63,   115,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,     0,     0,
       0,    61,    62,    63,   119,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,   134,     0,     0,     0,
      84,     9,    10,     0,     0,     0,   135,   136,     0,     0,
       0,   137,     0,     0,   138,   139,    61,    62,    63,     0,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    64,    65,    66,    67,    68,    -1,    -1,    -1,    -1,
      -1,    -1
  };

  const short
  Parser::yycheck_[] =
  {
       5,    12,    23,    11,    11,    25,     3,   131,   131,    12,
       3,     0,    33,    34,    35,    36,    21,    20,     8,     9,
      10,    49,    76,    12,     8,    53,    23,   151,   151,    11,
       3,     4,     5,     6,     7,    35,    36,    37,    59,    46,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    44,    77,     3,    78,    32,
      33,    34,     8,     9,    54,    55,   120,    13,    14,    51,
      54,    55,    18,     3,    47,    21,    22,    50,     0,    44,
     155,     8,   157,   103,    49,    12,    54,    55,    53,     3,
     110,    47,     3,   114,   169,   106,   171,    47,    49,   119,
      48,   176,    11,   124,     3,     8,     9,    10,    54,    55,
      51,    48,    52,    51,   135,   120,   137,   106,   139,     3,
       4,     5,     6,     7,    52,    24,    25,    26,    27,    28,
     141,    47,   153,    48,    12,   156,    12,    47,     3,    12,
      17,    48,    12,    15,    15,   166,   151,    16,    32,    33,
      34,    12,   141,    12,    21,    80,   151,   170,   145,   138,
      29,    30,    31,    47,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    15,    45,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    29,
      30,    31,    16,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    30,    31,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    29,    30,    31,    50,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    29,    30,    31,    50,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,     3,    -1,    -1,    -1,
      48,     8,     9,    -1,    -1,    -1,    13,    14,    -1,    -1,
      -1,    18,    -1,    -1,    21,    22,    29,    30,    31,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43
  };

  const signed char
  Parser::yystos_[] =
  {
       0,    54,    55,    59,    86,    87,    88,     0,    86,     8,
       9,    10,    60,    61,    62,    67,    68,    74,     3,     3,
       3,    87,    88,    11,    51,    11,    47,    61,     3,     4,
       5,     6,     7,    32,    33,    34,    47,    80,    83,     3,
      24,    25,    26,    27,    28,    69,    70,    71,    72,    69,
       3,    63,    64,    65,    47,    83,    83,    83,    83,    49,
      53,    29,    30,    31,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    49,    88,    11,    51,    48,
      52,    83,    84,    85,    48,    83,     3,    23,    82,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    50,    83,    12,    73,    74,    83,    69,
      51,    66,    65,    48,    52,    50,    47,    81,    69,    50,
      87,    88,    69,    11,    46,    83,    48,    69,    74,    12,
      75,    88,    83,    12,     3,    13,    14,    18,    21,    22,
      67,    76,    77,    78,    80,    47,    83,     3,    83,    85,
      83,    87,    88,    44,    84,    15,    17,    19,    77,    83,
      48,    75,    83,    75,    12,    16,    45,    79,    12,    20,
      83,    15,    75,    79,    75,    12,    15,    12,    75,    12
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    58,    59,    59,    60,    60,    61,    61,    62,    62,
      62,    63,    63,    64,    64,    65,    65,    66,    66,    67,
      67,    68,    69,    69,    69,    69,    70,    70,    70,    71,
      71,    72,    72,    73,    73,    74,    74,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    79,    79,    80,    80,    80,    81,
      81,    82,    82,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     3,     1,     1,     3,     1,     1,     9,     8,
       6,     0,     1,     1,     3,     3,     1,     0,     2,     1,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     5,
       4,     5,     3,     1,     3,     6,     4,     4,     1,     3,
       1,     3,     1,     1,     3,     4,     2,     5,     7,     5,
      10,     8,     1,     2,     0,     1,     1,     4,     4,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     4,     0,     1,     1,
       3,     1,     1,     1,     2,     0,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "INT_LITERAL", "REAL_LITERAL", "TRUE", "FALSE", "VAR", "TYPE", "ROUTINE",
  "IS", "END", "WHILE", "FOR", "LOOP", "REVERSE", "IN", "IF", "THEN",
  "ELSE", "PRINT", "RETURN", "SIZE", "KW_INTEGER", "KW_REAL", "KW_BOOLEAN",
  "ARRAY", "RECORD", "AND", "OR", "XOR", "NOT", "PLUS", "MINUS", "MUL",
  "DIV", "MOD", "LESS", "LESS_EQUAL", "GREATER", "GREATER_EQUAL", "EQUAL",
  "NOT_EQUAL", "ASSIGN", "RANGE", "ARROW", "LPAREN", "RPAREN", "LBRACKET",
  "RBRACKET", "COLON", "COMMA", "DOT", "SEMICOLON", "NEWLINE", "UMINUS",
  "UPLUS", "$accept", "program", "top_items", "top_item",
  "routine_declaration", "parameter_list", "parameters", "parameter",
  "optional_return_type", "declaration", "type_declaration", "type_spec",
  "primitive_type", "array_type", "record_type", "record_fields",
  "variable_declaration", "body", "body_items", "body_item", "statement",
  "optional_reverse", "modifiable_primary", "opt_empty_parens",
  "field_name", "expression", "argument_list", "expression_list", "sep",
  "seps", "opt_seps", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  Parser::yyrline_[] =
  {
       0,    86,    86,    94,   101,   107,   115,   119,   126,   138,
     146,   157,   161,   168,   174,   182,   186,   193,   197,   204,
     208,   215,   222,   226,   230,   234,   241,   242,   243,   247,
     251,   258,   266,   273,   279,   287,   291,   295,   302,   306,
     313,   319,   327,   331,   338,   342,   350,   358,   366,   378,
     386,   394,   402,   406,   413,   417,   424,   428,   432,   439,
     440,   444,   445,   449,   453,   457,   461,   465,   469,   473,
     477,   481,   485,   489,   493,   497,   501,   505,   509,   513,
     517,   521,   525,   529,   533,   537,   541,   552,   556,   563,
     569,   577,   578,   582,   583,   587,   588
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 6 "src/parser/Parser.yy"
} // yy
#line 2323 "src/parser/Parser.cpp"

#line 591 "src/parser/Parser.yy"

