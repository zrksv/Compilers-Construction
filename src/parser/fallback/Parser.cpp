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
#line 34 "src/parser/Parser.yy"

#include "ParserDriver.hpp"

yy::Parser::symbol_type yylex(ParserDriver& driver);

void yy::Parser::error(const Location& loc, const std::string& msg) {
    driver.error(loc, msg);
}

#line 56 "src/parser/fallback/Parser.cpp"


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
#line 149 "src/parser/fallback/Parser.cpp"

  /// Build a parser object.
  Parser::Parser (ParserDriver& driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      yy_lac_established_ (false),
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

      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.YY_MOVE_OR_COPY< std::unique_ptr<AstNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.YY_MOVE_OR_COPY< std::unique_ptr<Declaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.YY_MOVE_OR_COPY< std::unique_ptr<Expression> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter: // parameter
        value.YY_MOVE_OR_COPY< std::unique_ptr<ParameterDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.YY_MOVE_OR_COPY< std::unique_ptr<RoutineDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.YY_MOVE_OR_COPY< std::unique_ptr<Statement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.YY_MOVE_OR_COPY< std::unique_ptr<TypeDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.YY_MOVE_OR_COPY< std::unique_ptr<TypeNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_record_field: // record_field
      case symbol_kind::S_variable_declaration: // variable_declaration
        value.YY_MOVE_OR_COPY< std::unique_ptr<VariableDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.YY_MOVE_OR_COPY< std::vector<std::unique_ptr<AstNode>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.YY_MOVE_OR_COPY< std::vector<std::unique_ptr<Expression>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.YY_MOVE_OR_COPY< std::vector<std::unique_ptr<ParameterDeclaration>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.YY_MOVE_OR_COPY< std::vector<std::unique_ptr<VariableDeclaration>> > (YY_MOVE (that.value));
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

      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.move< std::unique_ptr<AstNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< std::unique_ptr<Declaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.move< std::unique_ptr<Expression> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter: // parameter
        value.move< std::unique_ptr<ParameterDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.move< std::unique_ptr<RoutineDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.move< std::unique_ptr<Statement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.move< std::unique_ptr<TypeDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.move< std::unique_ptr<TypeNode> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_record_field: // record_field
      case symbol_kind::S_variable_declaration: // variable_declaration
        value.move< std::unique_ptr<VariableDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.move< std::vector<std::unique_ptr<AstNode>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.move< std::vector<std::unique_ptr<Expression>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.move< std::vector<std::unique_ptr<ParameterDeclaration>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.move< std::vector<std::unique_ptr<VariableDeclaration>> > (YY_MOVE (that.value));
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

      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.copy< std::unique_ptr<AstNode> > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.copy< std::unique_ptr<Declaration> > (that.value);
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.copy< std::unique_ptr<Expression> > (that.value);
        break;

      case symbol_kind::S_parameter: // parameter
        value.copy< std::unique_ptr<ParameterDeclaration> > (that.value);
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.copy< std::unique_ptr<RoutineDeclaration> > (that.value);
        break;

      case symbol_kind::S_statement: // statement
        value.copy< std::unique_ptr<Statement> > (that.value);
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.copy< std::unique_ptr<TypeDeclaration> > (that.value);
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.copy< std::unique_ptr<TypeNode> > (that.value);
        break;

      case symbol_kind::S_record_field: // record_field
      case symbol_kind::S_variable_declaration: // variable_declaration
        value.copy< std::unique_ptr<VariableDeclaration> > (that.value);
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.copy< std::vector<std::unique_ptr<AstNode>> > (that.value);
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.copy< std::vector<std::unique_ptr<Expression>> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.copy< std::vector<std::unique_ptr<ParameterDeclaration>> > (that.value);
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.copy< std::vector<std::unique_ptr<VariableDeclaration>> > (that.value);
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

      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        value.move< std::unique_ptr<AstNode> > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< std::unique_ptr<Declaration> > (that.value);
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        value.move< std::unique_ptr<Expression> > (that.value);
        break;

      case symbol_kind::S_parameter: // parameter
        value.move< std::unique_ptr<ParameterDeclaration> > (that.value);
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        value.move< std::unique_ptr<RoutineDeclaration> > (that.value);
        break;

      case symbol_kind::S_statement: // statement
        value.move< std::unique_ptr<Statement> > (that.value);
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        value.move< std::unique_ptr<TypeDeclaration> > (that.value);
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        value.move< std::unique_ptr<TypeNode> > (that.value);
        break;

      case symbol_kind::S_record_field: // record_field
      case symbol_kind::S_variable_declaration: // variable_declaration
        value.move< std::unique_ptr<VariableDeclaration> > (that.value);
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        value.move< std::vector<std::unique_ptr<AstNode>> > (that.value);
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        value.move< std::vector<std::unique_ptr<Expression>> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        value.move< std::vector<std::unique_ptr<ParameterDeclaration>> > (that.value);
        break;

      case symbol_kind::S_record_fields: // record_fields
        value.move< std::vector<std::unique_ptr<VariableDeclaration>> > (that.value);
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

    // Discard the LAC context in case there still is one left from a
    // previous invocation.
    yy_lac_discard_ ("init");

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
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
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

      case symbol_kind::S_top_item: // top_item
      case symbol_kind::S_body_item: // body_item
        yylhs.value.emplace< std::unique_ptr<AstNode> > ();
        break;

      case symbol_kind::S_declaration: // declaration
        yylhs.value.emplace< std::unique_ptr<Declaration> > ();
        break;

      case symbol_kind::S_modifiable_primary: // modifiable_primary
      case symbol_kind::S_expression: // expression
        yylhs.value.emplace< std::unique_ptr<Expression> > ();
        break;

      case symbol_kind::S_parameter: // parameter
        yylhs.value.emplace< std::unique_ptr<ParameterDeclaration> > ();
        break;

      case symbol_kind::S_routine_declaration: // routine_declaration
        yylhs.value.emplace< std::unique_ptr<RoutineDeclaration> > ();
        break;

      case symbol_kind::S_statement: // statement
        yylhs.value.emplace< std::unique_ptr<Statement> > ();
        break;

      case symbol_kind::S_type_declaration: // type_declaration
        yylhs.value.emplace< std::unique_ptr<TypeDeclaration> > ();
        break;

      case symbol_kind::S_optional_return_type: // optional_return_type
      case symbol_kind::S_type_spec: // type_spec
      case symbol_kind::S_primitive_type: // primitive_type
      case symbol_kind::S_array_type: // array_type
      case symbol_kind::S_record_type: // record_type
        yylhs.value.emplace< std::unique_ptr<TypeNode> > ();
        break;

      case symbol_kind::S_record_field: // record_field
      case symbol_kind::S_variable_declaration: // variable_declaration
        yylhs.value.emplace< std::unique_ptr<VariableDeclaration> > ();
        break;

      case symbol_kind::S_top_items: // top_items
      case symbol_kind::S_body: // body
      case symbol_kind::S_body_items: // body_items
        yylhs.value.emplace< std::vector<std::unique_ptr<AstNode>> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
      case symbol_kind::S_expression_list: // expression_list
        yylhs.value.emplace< std::vector<std::unique_ptr<Expression>> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
      case symbol_kind::S_parameters: // parameters
        yylhs.value.emplace< std::vector<std::unique_ptr<ParameterDeclaration>> > ();
        break;

      case symbol_kind::S_record_fields: // record_fields
        yylhs.value.emplace< std::vector<std::unique_ptr<VariableDeclaration>> > ();
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
  case 2: // program: opt_seps
#line 92 "src/parser/Parser.yy"
    {
        driver.set_ast_root(std::make_unique<Program>(std::vector<std::unique_ptr<AstNode>>{}, Location{}));
    }
#line 956 "src/parser/fallback/Parser.cpp"
    break;

  case 3: // program: top_items opt_seps
#line 96 "src/parser/Parser.yy"
    {
        driver.set_ast_root(std::make_unique<Program>(YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), Location{}));
    }
#line 964 "src/parser/fallback/Parser.cpp"
    break;

  case 4: // program: seps top_items opt_seps
#line 100 "src/parser/Parser.yy"
    {
        driver.set_ast_root(std::make_unique<Program>(YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), Location{}));
    }
#line 972 "src/parser/fallback/Parser.cpp"
    break;

  case 5: // top_items: top_item
#line 107 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> items;
        if (auto item = YY_MOVE (yystack_[0].value.as < std::unique_ptr<AstNode> > ())) items.push_back(std::move(item));
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = std::move(items);
    }
#line 982 "src/parser/fallback/Parser.cpp"
    break;

  case 6: // top_items: top_items seps top_item
#line 113 "src/parser/Parser.yy"
    {
        auto items = YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<AstNode>> > ());
        if (auto item = YY_MOVE (yystack_[0].value.as < std::unique_ptr<AstNode> > ())) items.push_back(std::move(item));
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = std::move(items);
    }
#line 992 "src/parser/fallback/Parser.cpp"
    break;

  case 7: // top_items: top_items error
#line 119 "src/parser/Parser.yy"
    {
        yyerrok;
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ());
    }
#line 1001 "src/parser/fallback/Parser.cpp"
    break;

  case 8: // top_item: routine_declaration
#line 127 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<AstNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<RoutineDeclaration> > ());
    }
#line 1009 "src/parser/fallback/Parser.cpp"
    break;

  case 9: // top_item: declaration
#line 131 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<AstNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<Declaration> > ());
    }
#line 1017 "src/parser/fallback/Parser.cpp"
    break;

  case 10: // top_item: error
#line 135 "src/parser/Parser.yy"
    {
        yyerrok;
        yylhs.value.as < std::unique_ptr<AstNode> > () = nullptr;
    }
#line 1026 "src/parser/fallback/Parser.cpp"
    break;

  case 11: // routine_declaration: ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type IS body block_end
#line 143 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<RoutineDeclaration> > () = std::make_unique<RoutineDeclaration>(YY_MOVE (yystack_[7].value.as < std::string > ()), YY_MOVE (yystack_[5].value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > ()), YY_MOVE (yystack_[3].value.as < std::unique_ptr<TypeNode> > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), yystack_[8].location);
    }
#line 1034 "src/parser/fallback/Parser.cpp"
    break;

  case 12: // routine_declaration: ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type ARROW expression
#line 147 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<RoutineDeclaration> > () = std::make_unique<RoutineDeclaration>(YY_MOVE (yystack_[6].value.as < std::string > ()), YY_MOVE (yystack_[4].value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > ()), YY_MOVE (yystack_[2].value.as < std::unique_ptr<TypeNode> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[7].location);
    }
#line 1042 "src/parser/fallback/Parser.cpp"
    break;

  case 13: // routine_declaration: ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type
#line 151 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<RoutineDeclaration> > () = std::make_unique<RoutineDeclaration>(YY_MOVE (yystack_[4].value.as < std::string > ()), YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ()), yystack_[5].location);
    }
#line 1050 "src/parser/fallback/Parser.cpp"
    break;

  case 14: // routine_declaration: ROUTINE error IS body block_end
#line 155 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<RoutineDeclaration> > () = nullptr;
    }
#line 1058 "src/parser/fallback/Parser.cpp"
    break;

  case 15: // routine_declaration: ROUTINE error ARROW expression
#line 159 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<RoutineDeclaration> > () = nullptr;
    }
#line 1066 "src/parser/fallback/Parser.cpp"
    break;

  case 16: // routine_declaration: ROUTINE error
#line 163 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<RoutineDeclaration> > () = nullptr;
    }
#line 1074 "src/parser/fallback/Parser.cpp"
    break;

  case 17: // parameter_list: %empty
#line 170 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > () = std::vector<std::unique_ptr<ParameterDeclaration>>{};
    }
#line 1082 "src/parser/fallback/Parser.cpp"
    break;

  case 18: // parameter_list: parameters
#line 174 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > () = YY_MOVE (yystack_[0].value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > ());
    }
#line 1090 "src/parser/fallback/Parser.cpp"
    break;

  case 19: // parameters: parameter
#line 181 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<ParameterDeclaration>> params;
        params.push_back(YY_MOVE (yystack_[0].value.as < std::unique_ptr<ParameterDeclaration> > ()));
        yylhs.value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > () = std::move(params);
    }
#line 1100 "src/parser/fallback/Parser.cpp"
    break;

  case 20: // parameters: parameters COMMA parameter
#line 187 "src/parser/Parser.yy"
    {
        auto params = YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > ());
        params.push_back(YY_MOVE (yystack_[0].value.as < std::unique_ptr<ParameterDeclaration> > ()));
        yylhs.value.as < std::vector<std::unique_ptr<ParameterDeclaration>> > () = std::move(params);
    }
#line 1110 "src/parser/fallback/Parser.cpp"
    break;

  case 21: // parameter: IDENTIFIER COLON type_spec
#line 196 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<ParameterDeclaration> > () = std::make_unique<ParameterDeclaration>(YY_MOVE (yystack_[2].value.as < std::string > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ()), yystack_[2].location);
    }
#line 1118 "src/parser/fallback/Parser.cpp"
    break;

  case 22: // parameter: IDENTIFIER
#line 200 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<ParameterDeclaration> > () = std::make_unique<ParameterDeclaration>(YY_MOVE (yystack_[0].value.as < std::string > ()), yystack_[0].location);
    }
#line 1126 "src/parser/fallback/Parser.cpp"
    break;

  case 23: // optional_return_type: %empty
#line 207 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = nullptr;
    }
#line 1134 "src/parser/fallback/Parser.cpp"
    break;

  case 24: // optional_return_type: COLON type_spec
#line 211 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ());
    }
#line 1142 "src/parser/fallback/Parser.cpp"
    break;

  case 25: // declaration: variable_declaration
#line 218 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Declaration> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<VariableDeclaration> > ());
    }
#line 1150 "src/parser/fallback/Parser.cpp"
    break;

  case 26: // declaration: type_declaration
#line 222 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Declaration> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeDeclaration> > ());
    }
#line 1158 "src/parser/fallback/Parser.cpp"
    break;

  case 27: // type_declaration: TYPE IDENTIFIER IS type_spec
#line 229 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeDeclaration> > () = std::make_unique<TypeDeclaration>(YY_MOVE (yystack_[2].value.as < std::string > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ()), yystack_[3].location);
    }
#line 1166 "src/parser/fallback/Parser.cpp"
    break;

  case 28: // type_spec: primitive_type
#line 236 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ());
    }
#line 1174 "src/parser/fallback/Parser.cpp"
    break;

  case 29: // type_spec: IDENTIFIER
#line 240 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<IdentifierTypeNode>(YY_MOVE (yystack_[0].value.as < std::string > ()), yystack_[0].location);
    }
#line 1182 "src/parser/fallback/Parser.cpp"
    break;

  case 30: // type_spec: array_type
#line 244 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ());
    }
#line 1190 "src/parser/fallback/Parser.cpp"
    break;

  case 31: // type_spec: record_type
#line 248 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ());
    }
#line 1198 "src/parser/fallback/Parser.cpp"
    break;

  case 32: // primitive_type: KW_INTEGER
#line 254 "src/parser/Parser.yy"
               { yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<PrimitiveTypeNode>("integer", yystack_[0].location); }
#line 1204 "src/parser/fallback/Parser.cpp"
    break;

  case 33: // primitive_type: KW_REAL
#line 255 "src/parser/Parser.yy"
               { yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<PrimitiveTypeNode>("real", yystack_[0].location); }
#line 1210 "src/parser/fallback/Parser.cpp"
    break;

  case 34: // primitive_type: KW_BOOLEAN
#line 256 "src/parser/Parser.yy"
               { yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<PrimitiveTypeNode>("boolean", yystack_[0].location); }
#line 1216 "src/parser/fallback/Parser.cpp"
    break;

  case 35: // array_type: ARRAY LBRACKET expression RBRACKET type_spec
#line 261 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<ArrayTypeNode>(YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ()), yystack_[4].location);
    }
#line 1224 "src/parser/fallback/Parser.cpp"
    break;

  case 36: // array_type: ARRAY LBRACKET RBRACKET type_spec
#line 265 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<ArrayTypeNode>(nullptr, YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ()), yystack_[3].location);
    }
#line 1232 "src/parser/fallback/Parser.cpp"
    break;

  case 37: // record_type: RECORD record_fields opt_seps block_end
#line 272 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<RecordTypeNode>(YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<VariableDeclaration>> > ()), yystack_[3].location);
    }
#line 1240 "src/parser/fallback/Parser.cpp"
    break;

  case 38: // record_type: RECORD seps record_fields opt_seps block_end
#line 276 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<RecordTypeNode>(YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<VariableDeclaration>> > ()), yystack_[4].location);
    }
#line 1248 "src/parser/fallback/Parser.cpp"
    break;

  case 39: // record_type: RECORD opt_seps block_end
#line 280 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<TypeNode> > () = std::make_unique<RecordTypeNode>(std::vector<std::unique_ptr<VariableDeclaration>>{}, yystack_[2].location);
    }
#line 1256 "src/parser/fallback/Parser.cpp"
    break;

  case 40: // record_fields: record_field
#line 287 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<VariableDeclaration>> fields;
        if (auto field = YY_MOVE (yystack_[0].value.as < std::unique_ptr<VariableDeclaration> > ())) fields.push_back(std::move(field));
        yylhs.value.as < std::vector<std::unique_ptr<VariableDeclaration>> > () = std::move(fields);
    }
#line 1266 "src/parser/fallback/Parser.cpp"
    break;

  case 41: // record_fields: record_fields seps record_field
#line 293 "src/parser/Parser.yy"
    {
        auto fields = YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<VariableDeclaration>> > ());
        if (auto field = YY_MOVE (yystack_[0].value.as < std::unique_ptr<VariableDeclaration> > ())) fields.push_back(std::move(field));
        yylhs.value.as < std::vector<std::unique_ptr<VariableDeclaration>> > () = std::move(fields);
    }
#line 1276 "src/parser/fallback/Parser.cpp"
    break;

  case 42: // record_fields: record_fields error
#line 299 "src/parser/Parser.yy"
    {
        yyerrok;
        yylhs.value.as < std::vector<std::unique_ptr<VariableDeclaration>> > () = YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<VariableDeclaration>> > ());
    }
#line 1285 "src/parser/fallback/Parser.cpp"
    break;

  case 43: // record_field: variable_declaration
#line 307 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<VariableDeclaration> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<VariableDeclaration> > ());
    }
#line 1293 "src/parser/fallback/Parser.cpp"
    break;

  case 44: // record_field: error
#line 311 "src/parser/Parser.yy"
    {
        yyerrok;
        yylhs.value.as < std::unique_ptr<VariableDeclaration> > () = nullptr;
    }
#line 1302 "src/parser/fallback/Parser.cpp"
    break;

  case 45: // variable_declaration: VAR IDENTIFIER COLON type_spec IS expression
#line 319 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<VariableDeclaration> > () = std::make_unique<VariableDeclaration>(YY_MOVE (yystack_[4].value.as < std::string > ()), YY_MOVE (yystack_[2].value.as < std::unique_ptr<TypeNode> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[5].location);
    }
#line 1310 "src/parser/fallback/Parser.cpp"
    break;

  case 46: // variable_declaration: VAR IDENTIFIER COLON type_spec
#line 323 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<VariableDeclaration> > () = std::make_unique<VariableDeclaration>(YY_MOVE (yystack_[2].value.as < std::string > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<TypeNode> > ()), nullptr, yystack_[3].location);
    }
#line 1318 "src/parser/fallback/Parser.cpp"
    break;

  case 47: // variable_declaration: VAR IDENTIFIER IS expression
#line 327 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<VariableDeclaration> > () = std::make_unique<VariableDeclaration>(YY_MOVE (yystack_[2].value.as < std::string > ()), nullptr, YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[3].location);
    }
#line 1326 "src/parser/fallback/Parser.cpp"
    break;

  case 48: // body: opt_seps
#line 334 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = std::vector<std::unique_ptr<AstNode>>{};
    }
#line 1334 "src/parser/fallback/Parser.cpp"
    break;

  case 49: // body: body_items opt_seps
#line 338 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ());
    }
#line 1342 "src/parser/fallback/Parser.cpp"
    break;

  case 50: // body: seps body_items opt_seps
#line 342 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ());
    }
#line 1350 "src/parser/fallback/Parser.cpp"
    break;

  case 51: // body_items: body_item
#line 349 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<AstNode>> items;
        if (auto item = YY_MOVE (yystack_[0].value.as < std::unique_ptr<AstNode> > ())) items.push_back(std::move(item));
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = std::move(items);
    }
#line 1360 "src/parser/fallback/Parser.cpp"
    break;

  case 52: // body_items: body_items seps body_item
#line 355 "src/parser/Parser.yy"
    {
        auto items = YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<AstNode>> > ());
        if (auto item = YY_MOVE (yystack_[0].value.as < std::unique_ptr<AstNode> > ())) items.push_back(std::move(item));
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = std::move(items);
    }
#line 1370 "src/parser/fallback/Parser.cpp"
    break;

  case 53: // body_items: body_items error
#line 361 "src/parser/Parser.yy"
    {
        yyerrok;
        yylhs.value.as < std::vector<std::unique_ptr<AstNode>> > () = YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ());
    }
#line 1379 "src/parser/fallback/Parser.cpp"
    break;

  case 54: // body_item: statement
#line 369 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<AstNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<Statement> > ());
    }
#line 1387 "src/parser/fallback/Parser.cpp"
    break;

  case 55: // body_item: declaration
#line 373 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<AstNode> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<Declaration> > ());
    }
#line 1395 "src/parser/fallback/Parser.cpp"
    break;

  case 56: // body_item: error
#line 377 "src/parser/Parser.yy"
    {
        yyerrok;
        yylhs.value.as < std::unique_ptr<AstNode> > () = nullptr;
    }
#line 1404 "src/parser/fallback/Parser.cpp"
    break;

  case 57: // statement: modifiable_primary ASSIGN expression
#line 385 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<AssignStatement>(YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1412 "src/parser/fallback/Parser.cpp"
    break;

  case 58: // statement: IDENTIFIER LPAREN argument_list RPAREN
#line 389 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<RoutineCallStmt>(YY_MOVE (yystack_[3].value.as < std::string > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<Expression>> > ()), yystack_[3].location);
    }
#line 1420 "src/parser/fallback/Parser.cpp"
    break;

  case 59: // statement: PRINT expression_list
#line 393 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<PrintStatement>(YY_MOVE (yystack_[0].value.as < std::vector<std::unique_ptr<Expression>> > ()), yystack_[1].location);
    }
#line 1428 "src/parser/fallback/Parser.cpp"
    break;

  case 60: // statement: IF expression THEN body block_end
#line 397 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<IfStatement>(YY_MOVE (yystack_[3].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), std::vector<std::unique_ptr<AstNode>>{}, yystack_[4].location);
    }
#line 1436 "src/parser/fallback/Parser.cpp"
    break;

  case 61: // statement: IF expression THEN body ELSE body block_end
#line 401 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<IfStatement>(YY_MOVE (yystack_[5].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[3].value.as < std::vector<std::unique_ptr<AstNode>> > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), yystack_[6].location);
    }
#line 1444 "src/parser/fallback/Parser.cpp"
    break;

  case 62: // statement: WHILE expression LOOP body block_end
#line 405 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<WhileStatement>(YY_MOVE (yystack_[3].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), yystack_[4].location);
    }
#line 1452 "src/parser/fallback/Parser.cpp"
    break;

  case 63: // statement: FOR IDENTIFIER IN expression RANGE expression optional_reverse LOOP body block_end
#line 409 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<ForStatement>(YY_MOVE (yystack_[8].value.as < std::string > ()), YY_MOVE (yystack_[6].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[4].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[3].value.as < bool > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), yystack_[9].location);
    }
#line 1460 "src/parser/fallback/Parser.cpp"
    break;

  case 64: // statement: FOR IDENTIFIER IN expression optional_reverse LOOP body block_end
#line 413 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<ForStatement>(YY_MOVE (yystack_[6].value.as < std::string > ()), YY_MOVE (yystack_[4].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[3].value.as < bool > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<AstNode>> > ()), yystack_[7].location);
    }
#line 1468 "src/parser/fallback/Parser.cpp"
    break;

  case 65: // statement: IF error THEN body block_end
#line 417 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1476 "src/parser/fallback/Parser.cpp"
    break;

  case 66: // statement: IF error THEN body ELSE body block_end
#line 421 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1484 "src/parser/fallback/Parser.cpp"
    break;

  case 67: // statement: IF error sep body block_end
#line 425 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1492 "src/parser/fallback/Parser.cpp"
    break;

  case 68: // statement: IF error sep body ELSE body block_end
#line 429 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1500 "src/parser/fallback/Parser.cpp"
    break;

  case 69: // statement: WHILE error LOOP body block_end
#line 433 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1508 "src/parser/fallback/Parser.cpp"
    break;

  case 70: // statement: WHILE error sep body block_end
#line 437 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1516 "src/parser/fallback/Parser.cpp"
    break;

  case 71: // statement: FOR error LOOP body block_end
#line 441 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1524 "src/parser/fallback/Parser.cpp"
    break;

  case 72: // statement: FOR error sep body block_end
#line 445 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = nullptr;
    }
#line 1532 "src/parser/fallback/Parser.cpp"
    break;

  case 73: // statement: RETURN
#line 449 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<ReturnStatement>(nullptr, yystack_[0].location);
    }
#line 1540 "src/parser/fallback/Parser.cpp"
    break;

  case 74: // statement: RETURN expression
#line 453 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Statement> > () = std::make_unique<ReturnStatement>(YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1548 "src/parser/fallback/Parser.cpp"
    break;

  case 76: // block_end: END error
#line 461 "src/parser/Parser.yy"
    {
        yyerrok;
    }
#line 1556 "src/parser/fallback/Parser.cpp"
    break;

  case 77: // optional_reverse: %empty
#line 468 "src/parser/Parser.yy"
    {
        yylhs.value.as < bool > () = false;
    }
#line 1564 "src/parser/fallback/Parser.cpp"
    break;

  case 78: // optional_reverse: REVERSE
#line 472 "src/parser/Parser.yy"
    {
        yylhs.value.as < bool > () = true;
    }
#line 1572 "src/parser/fallback/Parser.cpp"
    break;

  case 79: // modifiable_primary: IDENTIFIER
#line 479 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<Identifier>(YY_MOVE (yystack_[0].value.as < std::string > ()), yystack_[0].location);
    }
#line 1580 "src/parser/fallback/Parser.cpp"
    break;

  case 80: // modifiable_primary: modifiable_primary DOT field_name opt_empty_parens
#line 483 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<FieldAccessExpr>(YY_MOVE (yystack_[3].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[1].value.as < std::string > ()), yystack_[2].location);
    }
#line 1588 "src/parser/fallback/Parser.cpp"
    break;

  case 81: // modifiable_primary: modifiable_primary LBRACKET expression RBRACKET
#line 487 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<ElementAccessExpr>(YY_MOVE (yystack_[3].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[1].value.as < std::unique_ptr<Expression> > ()), yystack_[2].location);
    }
#line 1596 "src/parser/fallback/Parser.cpp"
    break;

  case 84: // field_name: IDENTIFIER
#line 498 "src/parser/Parser.yy"
               { yylhs.value.as < std::string > () = YY_MOVE (yystack_[0].value.as < std::string > ()); }
#line 1602 "src/parser/fallback/Parser.cpp"
    break;

  case 85: // field_name: SIZE
#line 499 "src/parser/Parser.yy"
               { yylhs.value.as < std::string > () = "size"; }
#line 1608 "src/parser/fallback/Parser.cpp"
    break;

  case 86: // expression: INT_LITERAL
#line 504 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<IntLiteral>(YY_MOVE (yystack_[0].value.as < int > ()), yystack_[0].location);
    }
#line 1616 "src/parser/fallback/Parser.cpp"
    break;

  case 87: // expression: REAL_LITERAL
#line 508 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<RealLiteral>(YY_MOVE (yystack_[0].value.as < double > ()), yystack_[0].location);
    }
#line 1624 "src/parser/fallback/Parser.cpp"
    break;

  case 88: // expression: TRUE
#line 512 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BoolLiteral>(true, yystack_[0].location);
    }
#line 1632 "src/parser/fallback/Parser.cpp"
    break;

  case 89: // expression: FALSE
#line 516 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BoolLiteral>(false, yystack_[0].location);
    }
#line 1640 "src/parser/fallback/Parser.cpp"
    break;

  case 90: // expression: modifiable_primary
#line 520 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ());
    }
#line 1648 "src/parser/fallback/Parser.cpp"
    break;

  case 91: // expression: LPAREN expression RPAREN
#line 524 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = YY_MOVE (yystack_[1].value.as < std::unique_ptr<Expression> > ());
    }
#line 1656 "src/parser/fallback/Parser.cpp"
    break;

  case 92: // expression: expression PLUS expression
#line 528 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Plus, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1664 "src/parser/fallback/Parser.cpp"
    break;

  case 93: // expression: expression MINUS expression
#line 532 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Minus, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1672 "src/parser/fallback/Parser.cpp"
    break;

  case 94: // expression: expression MUL expression
#line 536 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Mul, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1680 "src/parser/fallback/Parser.cpp"
    break;

  case 95: // expression: expression DIV expression
#line 540 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Div, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1688 "src/parser/fallback/Parser.cpp"
    break;

  case 96: // expression: expression MOD expression
#line 544 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Mod, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1696 "src/parser/fallback/Parser.cpp"
    break;

  case 97: // expression: expression AND expression
#line 548 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::And, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1704 "src/parser/fallback/Parser.cpp"
    break;

  case 98: // expression: expression OR expression
#line 552 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Or, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1712 "src/parser/fallback/Parser.cpp"
    break;

  case 99: // expression: expression XOR expression
#line 556 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Xor, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1720 "src/parser/fallback/Parser.cpp"
    break;

  case 100: // expression: expression LESS expression
#line 560 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Less, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1728 "src/parser/fallback/Parser.cpp"
    break;

  case 101: // expression: expression LESS_EQUAL expression
#line 564 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::LessEqual, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1736 "src/parser/fallback/Parser.cpp"
    break;

  case 102: // expression: expression GREATER expression
#line 568 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Greater, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1744 "src/parser/fallback/Parser.cpp"
    break;

  case 103: // expression: expression GREATER_EQUAL expression
#line 572 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::GreaterEqual, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1752 "src/parser/fallback/Parser.cpp"
    break;

  case 104: // expression: expression EQUAL expression
#line 576 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::Equal, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1760 "src/parser/fallback/Parser.cpp"
    break;

  case 105: // expression: expression NOT_EQUAL expression
#line 580 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<BinaryExpr>(BinOp::NotEqual, YY_MOVE (yystack_[2].value.as < std::unique_ptr<Expression> > ()), YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1768 "src/parser/fallback/Parser.cpp"
    break;

  case 106: // expression: PLUS expression
#line 584 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<UnaryExpr>(UnaryOp::Plus, YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1776 "src/parser/fallback/Parser.cpp"
    break;

  case 107: // expression: MINUS expression
#line 588 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<UnaryExpr>(UnaryOp::Minus, YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1784 "src/parser/fallback/Parser.cpp"
    break;

  case 108: // expression: NOT expression
#line 592 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<UnaryExpr>(UnaryOp::Not, YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()), yystack_[1].location);
    }
#line 1792 "src/parser/fallback/Parser.cpp"
    break;

  case 109: // expression: IDENTIFIER LPAREN argument_list RPAREN
#line 596 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::unique_ptr<Expression> > () = std::make_unique<RoutineCallExpr>(YY_MOVE (yystack_[3].value.as < std::string > ()), YY_MOVE (yystack_[1].value.as < std::vector<std::unique_ptr<Expression>> > ()), yystack_[3].location);
    }
#line 1800 "src/parser/fallback/Parser.cpp"
    break;

  case 110: // argument_list: %empty
#line 603 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<Expression>> > () = std::vector<std::unique_ptr<Expression>>{};
    }
#line 1808 "src/parser/fallback/Parser.cpp"
    break;

  case 111: // argument_list: expression_list
#line 607 "src/parser/Parser.yy"
    {
        yylhs.value.as < std::vector<std::unique_ptr<Expression>> > () = YY_MOVE (yystack_[0].value.as < std::vector<std::unique_ptr<Expression>> > ());
    }
#line 1816 "src/parser/fallback/Parser.cpp"
    break;

  case 112: // expression_list: expression
#line 614 "src/parser/Parser.yy"
    {
        std::vector<std::unique_ptr<Expression>> list;
        list.push_back(YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()));
        yylhs.value.as < std::vector<std::unique_ptr<Expression>> > () = std::move(list);
    }
#line 1826 "src/parser/fallback/Parser.cpp"
    break;

  case 113: // expression_list: expression_list COMMA expression
#line 620 "src/parser/Parser.yy"
    {
        auto list = YY_MOVE (yystack_[2].value.as < std::vector<std::unique_ptr<Expression>> > ());
        list.push_back(YY_MOVE (yystack_[0].value.as < std::unique_ptr<Expression> > ()));
        yylhs.value.as < std::vector<std::unique_ptr<Expression>> > () = std::move(list);
    }
#line 1836 "src/parser/fallback/Parser.cpp"
    break;


#line 1840 "src/parser/fallback/Parser.cpp"

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
      yy_lac_discard_ ("error recovery");
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

#if YYDEBUG
    // Execute LAC once. We don't care if it is successful, we
    // only do it for the sake of debugging output.
    if (!yyparser_.yy_lac_established_)
      yyparser_.yy_lac_check_ (yyla_.kind ());
#endif

    for (int yyx = 0; yyx < YYNTOKENS; ++yyx)
      {
        symbol_kind_type yysym = YY_CAST (symbol_kind_type, yyx);
        if (yysym != symbol_kind::S_YYerror
            && yysym != symbol_kind::S_YYUNDEF
            && yyparser_.yy_lac_check_ (yysym))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }




  bool
  Parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  Parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (yy_lac_established_)
      return true;
    else
      {
#if YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
  }

  // Discard any previous initial lookahead context.
  void
  Parser::yy_lac_discard_ (const char* event)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << event << '\n';
        yy_lac_established_ = false;
      }
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
         In the first two cases, it might appear that the current syntax
         error should have been detected in the previous state when
         yy_lac_check was invoked.  However, at that time, there might
         have been a different syntax error that discarded a different
         initial context during error recovery, leaving behind the
         current lookahead.
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


  const signed char Parser::yypact_ninf_ = -49;

  const signed char Parser::yytable_ninf_ = -120;

  const short
  Parser::yypact_[] =
  {
      43,    28,     5,    17,   105,  1755,  1811,    11,    46,    87,
     160,   166,   212,   249,  1827,    84,    23,    -6,    26,   103,
      -5,   -49,   184,    84,    39,    46,  1850,  2409,  1703,  1703,
    2378,  2409,     6,   188,    58,   412,   652,   698,   744,   790,
    2409,  2409,  2409,  2409,   606,  1636,  1873,  1878,  1891,  1896,
      29,   365,   163,  1952,  1957,  1970,   305,   368,   274,  1692,
     245,  2254,  2409,  2292,   417,    67,   424,   700,   746,   -18,
    2340,    -8,  1707,   -38,    38,    83,   120,  2402,   836,   882,
     928,  2500,  2409,    18,  2409,  2409,  2409,  2409,  2409,  2409,
    2409,  2409,  2409,  2409,  2409,  2409,  2409,  2409,  1654,   619,
     668,   707,   714,   569,    67,  2409,  2402,    62,  2424,    66,
      88,    90,  2449,  2148,    98,  2236,  1975,   307,   792,  2340,
      36,  2409,   424,  1703,   481,   119,    89,   121,   974,  2464,
     244,   300,   356,  1020,  1066,  1112,  1158,  1204,  1250,  1296,
    1342,  1434,  1461,  1502,  1529,  1570,  1597,  1703,  2482,   806,
     569,    67,   668,  2031,  1680,   107,  2378,  2378,  2378,  2378,
    2378,  2409,  2356,  2356,  2356,  2409,  2036,   838,  2280,   136,
     158,  1703,   485,   182,  1388,   468,   131,   514,  2049,  1703,
     852,  2054,    67,   884,    67,    67,    67,    67,    67,  2103,
     150,   199,   213,  2192,   573,  2378,  2409,   560,  2110,  2115,
     930,   976,  1022,  1068,  1114,   171,  2409,   176,  2378,  1160,
    2378,  1206,  2378,  1252,    67,  1751,  2388,  2378,    67,    67,
      67,   309,   201,    67,  1298,  1344,  1390,  2378,  1446,    67,
    1473
  };

  const signed char
  Parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0
  };

  const short
  Parser::yypgoto_[] =
  {
     -49,   -49,   208,   206,   -49,   -49,   -49,   110,   -49,     7,
     -49,   -28,   -49,   -49,   -49,   133,    91,   -48,    45,   177,
     132,   -49,     8,    42,   -30,   -49,   -49,   -25,   146,   192,
       4,    24,    25
  };

  const unsigned char
  Parser::yydefgoto_[] =
  {
       0,     7,     8,     9,    10,    74,    75,    76,   172,    64,
      12,    52,    53,    54,    55,   100,   101,    13,    65,    66,
      67,    68,   117,   207,    44,   177,   132,   113,   126,   127,
      14,    70,    71
  };

  const short
  Parser::yytable_[] =
  {
      69,    56,    45,   102,   -48,    27,    72,    11,    17,    73,
     -22,    21,   -48,   123,   -22,    78,    79,    80,    81,    26,
      18,   130,    11,    -2,    15,    16,   121,    26,   -10,   -10,
      11,    82,    23,    24,   108,    83,   112,    29,   115,    -3,
      69,   131,    32,  -118,     1,    28,  -118,    22,   -49,    23,
      34,     2,     3,     4,   -17,   102,   -49,   129,    -4,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   148,    26,   103,   104,   156,    98,   116,
     154,   159,   -10,   -10,  -119,     1,   124,    -5,    -5,    69,
     119,   120,     2,     3,     4,   170,   168,     5,     6,   -59,
       5,     6,   102,   -16,   -16,   161,    19,    26,    20,   162,
     -59,   157,   153,   160,    30,   163,     5,     6,   -59,   178,
       5,     6,    73,    26,   150,   151,    69,    69,    69,    69,
      69,   -18,    69,    69,    69,   125,   189,   174,     5,     6,
     193,    -5,    -5,   194,     5,     6,   119,   169,   -50,    31,
     165,   198,   -59,   -59,    26,   183,   -50,   -16,   -16,   181,
      -8,    -8,   116,   -46,   -46,    69,    -9,    -9,   -19,  -111,
     208,   215,   -19,   165,   105,   -46,   150,   182,    69,   197,
      69,   216,    69,   -46,    -7,    -7,   -78,    69,    -6,    -6,
     199,   217,   200,   201,   202,   203,   204,    69,   209,   211,
     213,   184,   185,   186,   187,   188,   -21,   190,   191,   192,
     -21,   116,   -26,   -26,    -8,    -8,   227,   -46,   -46,   210,
      -9,    -9,   221,    25,   -26,   116,   224,   225,   226,    33,
     -20,   228,   -26,   212,   -20,   173,   152,   230,    -7,    -7,
     214,   180,    -6,    -6,   -84,   -84,   109,   122,   110,   -25,
     -25,   167,   155,   218,   114,   219,   -84,   220,   222,   -84,
     -84,   -25,   223,   -84,   -84,     0,   -26,   -26,     0,   -25,
       0,     0,   229,   -84,   -84,   -84,     0,   -84,   -84,   -84,
     -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,
       0,   -84,   -84,   -84,   -84,     0,   -84,   -84,   -84,   -84,
     -85,   -85,     0,   -25,   -25,   -27,   -27,   -14,   -14,   -11,
     -11,     0,   -85,     0,     0,   -85,   -85,   -27,   -79,   -85,
     -85,   106,     0,   -79,     0,   -27,     0,   -79,     0,   -85,
     -85,   -85,     0,   -85,   -85,   -85,   -85,   -85,   -85,   -85,
     -85,   -85,   -85,   -85,   -85,   -85,     0,   -85,   -85,   -85,
     -85,     0,   -85,   -85,   -85,   -85,   -82,   -82,     0,   -27,
     -27,   -14,   -14,   -11,   -11,     0,    99,     0,   -82,   -56,
       0,   -82,   -82,     2,     0,   -82,   -82,  -118,     0,     0,
     -56,     0,     0,     0,     0,   -82,   -82,   -82,   -56,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,     0,   176,   -82,   -82,   -82,     0,   -82,   -82,
     -82,   -82,   -79,   -79,     0,     0,     0,     0,   -55,     5,
       6,     0,   -56,   -56,   -79,   118,     0,   -79,   -79,   -55,
       0,   -79,   -79,     0,     0,     0,  -118,   -55,     0,     0,
       0,   -79,   -79,   -79,  -118,   -79,   -79,   -79,   -79,   -79,
     -79,   -79,   -79,   -79,   -79,   -79,     0,   -79,     0,    77,
     -79,   -79,   -79,     0,   -79,   -79,   -79,   -79,   -81,   -81,
       0,   -55,   -55,     0,     0,     0,     0,     0,     5,     6,
     -81,   -23,   -23,   -81,   -81,   -13,   -13,   -81,   -81,     0,
       0,     0,   -23,     0,     0,     0,   195,   -81,   -81,   -81,
       0,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,   -81,   -80,   -80,   -81,   -81,   -81,     0,
     -81,   -81,   -81,   -81,     0,     0,   -80,   -23,     0,   -80,
     -80,   196,   171,   -80,   -80,   -23,   -23,     0,     0,   -13,
     -13,     0,     0,   -80,   -80,   -80,     0,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,
     -83,   -83,   -80,   -80,   -80,     0,   -80,   -80,   -80,   -80,
      99,     0,   -83,   -24,   -24,   -83,   -83,     2,     0,   -83,
     -83,  -119,     0,     0,   -24,     0,     0,     0,     0,   -83,
     -83,   -83,     0,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -90,   -90,   -83,   -83,
     -83,     0,   -83,   -83,   -83,   -83,     0,     0,   -90,   -24,
     -44,   -90,   -90,     5,     6,   -90,   -90,   -24,   -24,     0,
       0,   -44,     0,     0,     0,   -90,   -90,   -90,     0,   -90,
     -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,
       0,   -90,   -86,   -86,   -90,    82,   -90,     0,   -90,    83,
     -90,   -90,     0,     0,   -86,     0,     0,   -86,   -86,   149,
       0,   -86,   -86,   -44,   -44,     0,     0,     0,     0,     0,
    -118,   -86,   -86,   -86,     0,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,     0,   -86,   -87,   -87,
     -86,   -51,   -86,     0,   -86,     0,   -86,   -86,   -40,     0,
     -87,     0,   -51,   -87,   -87,   -43,     0,   -87,   -87,   -40,
     -51,     0,     5,     6,     0,     0,   -43,   -87,   -87,   -87,
       0,   -87,   -87,   -87,   -87,   -87,   -87,   -87,   -87,   -87,
     -87,   -87,     0,   -87,   -88,   -88,   -87,   -54,   -87,     0,
     -87,     0,   -87,   -87,   -51,   -51,   -88,     0,   -54,   -88,
     -88,   -40,   -40,   -88,   -88,     0,   -54,     0,   -43,   -43,
       0,     0,     0,   -88,   -88,   -88,     0,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,     0,   -88,
     -89,   -89,   -88,   -53,   -88,     0,   -88,     0,   -88,   -88,
     -54,   -54,   -89,     0,   -53,   -89,   -89,   -42,     0,   -89,
     -89,     0,   -53,     0,     0,     0,     0,     0,   -42,   -89,
     -89,   -89,     0,   -89,   -89,   -89,   -89,   -89,   -89,   -89,
     -89,   -89,   -89,   -89,     0,   -89,  -108,  -108,   -89,   -52,
     -89,     0,   -89,     0,   -89,   -89,   -53,   -53,  -108,     0,
     -52,  -108,  -108,   -41,     0,  -108,  -108,     0,   -52,     0,
     -42,   -42,     0,     0,   -41,  -108,  -108,  -108,     0,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
       0,  -108,  -106,  -106,  -108,   -58,  -108,     0,  -108,     0,
    -108,  -108,   -52,   -52,  -106,     0,   -58,  -106,  -106,     0,
       0,  -106,  -106,     0,   -58,     0,   -41,   -41,     0,     0,
       0,  -106,  -106,  -106,     0,  -106,  -106,  -106,  -106,  -106,
    -106,  -106,  -106,  -106,  -106,  -106,     0,  -106,  -107,  -107,
    -106,   -69,  -106,     0,  -106,     0,  -106,  -106,   -58,   -58,
    -107,     0,   -69,  -107,  -107,     0,     0,  -107,  -107,     0,
     -69,     0,     0,     0,     0,     0,     0,  -107,  -107,  -107,
       0,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,     0,  -107,   -91,   -91,  -107,   -70,  -107,     0,
    -107,     0,  -107,  -107,   -69,   -69,   -91,     0,   -70,   -91,
     -91,     0,     0,   -91,   -91,     0,   -70,     0,     0,     0,
       0,     0,     0,   -91,   -91,   -91,     0,   -91,   -91,   -91,
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,     0,   -91,
     -97,   -97,   -91,   -62,   -91,     0,   -91,     0,   -91,   -91,
     -70,   -70,   -97,     0,   -62,   -97,   -97,     0,     0,   -97,
     -97,     0,   -62,     0,     0,     0,     0,     0,     0,   -97,
     -97,   -97,     0,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     0,   -97,   -98,   -98,   -97,   -71,
     -97,     0,   -97,     0,   -97,   -97,   -62,   -62,   -98,     0,
     -71,   -98,   -98,     0,     0,   -98,   -98,     0,   -71,     0,
       0,     0,     0,     0,     0,   -98,   -98,   -98,     0,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
       0,   -98,   -99,   -99,   -98,   -72,   -98,     0,   -98,     0,
     -98,   -98,   -71,   -71,   -99,     0,   -72,   -99,   -99,     0,
       0,   -99,   -99,     0,   -72,     0,     0,     0,     0,     0,
       0,   -99,   -99,   -99,     0,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,     0,   -99,   -92,   -92,
     -99,   -65,   -99,     0,   -99,     0,   -99,   -99,   -72,   -72,
     -92,     0,   -65,   -92,   -92,     0,     0,   -92,   -92,     0,
     -65,     0,     0,     0,     0,     0,     0,   -92,   -92,   -92,
       0,   -92,   -92,    89,    90,    91,   -92,   -92,   -92,   -92,
     -92,   -92,     0,   -92,   -93,   -93,   -92,   -67,   -92,     0,
     -92,     0,   -92,   -92,   -65,   -65,   -93,     0,   -67,   -93,
     -93,     0,     0,   -93,   -93,     0,   -67,     0,     0,     0,
       0,     0,     0,   -93,   -93,   -93,     0,   -93,   -93,    89,
      90,    91,   -93,   -93,   -93,   -93,   -93,   -93,     0,   -93,
     -94,   -94,   -93,   -60,   -93,     0,   -93,     0,   -93,   -93,
     -67,   -67,   -94,     0,   -60,   -94,   -94,     0,     0,   -94,
     -94,     0,   -60,     0,     0,     0,     0,     0,     0,   -94,
     -94,   -94,     0,   -94,   -94,   -94,   -94,   -94,   -94,   -94,
     -94,   -94,   -94,   -94,     0,   -94,   -95,   -95,   -94,   -66,
     -94,     0,   -94,     0,   -94,   -94,   -60,   -60,   -95,     0,
     -66,   -95,   -95,     0,     0,   -95,   -95,     0,   -66,     0,
       0,     0,     0,     0,     0,   -95,   -95,   -95,     0,   -95,
     -95,   -95,   -95,   -95,   -95,   -95,   -95,   -95,   -95,   -95,
       0,   -95,   -96,   -96,   -95,   -68,   -95,     0,   -95,     0,
     -95,   -95,   -66,   -66,   -96,     0,   -68,   -96,   -96,     0,
       0,   -96,   -96,     0,   -68,     0,     0,     0,     0,     0,
       0,   -96,   -96,   -96,     0,   -96,   -96,   -96,   -96,   -96,
     -96,   -96,   -96,   -96,   -96,   -96,     0,   -96,  -109,  -109,
     -96,   -61,   -96,     0,   -96,     0,   -96,   -96,   -68,   -68,
    -109,     0,   -61,  -109,  -109,     0,     0,  -109,  -109,     0,
     -61,     0,     0,     0,     0,     0,     0,  -109,  -109,  -109,
       0,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,     0,  -109,  -100,  -100,  -109,     0,  -109,     0,
    -109,     0,  -109,  -109,   -61,   -61,  -100,   -64,     0,  -100,
    -100,     0,     0,  -100,  -100,     0,     0,     0,   -64,     0,
       0,  -101,  -101,  -100,  -100,  -100,   -64,    87,    88,    89,
      90,    91,     0,  -101,   -63,     0,  -101,  -101,     0,  -100,
    -101,  -101,  -100,     0,  -100,   -63,  -100,     0,  -100,  -100,
    -101,  -101,  -101,   -63,    87,    88,    89,    90,    91,     0,
     -64,   -64,  -102,  -102,     0,     0,  -101,     0,     0,  -101,
       0,  -101,     0,  -101,  -102,  -101,  -101,  -102,  -102,     0,
       0,  -102,  -102,     0,     0,     0,     0,   -63,   -63,  -103,
    -103,  -102,  -102,  -102,     0,    87,    88,    89,    90,    91,
       0,  -103,     0,     0,  -103,  -103,     0,  -102,  -103,  -103,
    -102,     0,  -102,     0,  -102,     0,  -102,  -102,  -103,  -103,
    -103,     0,    87,    88,    89,    90,    91,     0,     0,     0,
    -104,  -104,     0,     0,  -103,     0,     0,  -103,     0,  -103,
       0,  -103,  -104,  -103,  -103,  -104,  -104,     0,     0,  -104,
    -104,     0,     0,     0,     0,     0,     0,  -105,  -105,  -104,
    -104,  -104,     0,    87,    88,    89,    90,    91,     0,  -105,
       0,     0,  -105,  -105,     0,  -104,  -105,  -105,  -104,     0,
    -104,     0,  -104,     0,  -104,  -104,  -105,  -105,  -105,     0,
      87,    88,    89,    90,    91,     0,   -47,   -47,     0,     0,
       0,     0,  -105,     0,     0,  -105,     0,  -105,   -47,  -105,
       0,  -105,  -105,     0,     0,     0,   -47,    35,    36,    37,
      38,    39,     0,     0,     0,    84,    85,    86,     0,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
     -45,   -45,     0,     0,     0,     0,    40,    41,    42,     0,
     -47,   -47,   -45,   107,     0,    35,    36,    37,    38,    39,
     -45,    43,     0,     0,   147,     0,    46,   -15,   -15,    84,
      85,    86,     0,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    40,    41,    42,    47,    48,    49,
      50,    51,     0,     0,   -45,   -45,    84,    85,    86,    43,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,   -12,   -12,     0,     0,  -114,  -114,     0,  -114,     0,
       0,   -15,   -15,  -114,  -114,  -114,     0,  -114,  -114,  -114,
       0,     0,     0,  -114,     0,  -114,  -114,  -114,     0,     0,
      84,    85,    86,     0,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   -12,   -12,     0,     0,  -114,
    -114,  -115,  -115,     0,  -115,     0,     0,     0,     0,  -115,
    -115,  -115,     0,  -115,  -115,  -115,     0,  -116,  -116,  -115,
    -116,  -115,  -115,  -115,     0,  -116,  -116,  -116,     0,  -116,
    -116,  -116,     0,     0,     0,  -116,     0,  -116,  -116,  -116,
    -117,  -117,     0,  -117,     0,     0,     0,     0,  -117,  -117,
    -117,     0,  -117,  -117,  -117,  -115,  -115,     0,  -117,     0,
    -117,  -117,  -117,   -29,   -29,     0,     0,     0,   -32,   -32,
       0,  -116,  -116,     0,   -29,   -29,     0,     0,     0,   -32,
     -32,   -33,   -33,   -29,     0,     0,   -34,   -34,   -32,     0,
       0,     0,   -33,   -33,  -117,  -117,     0,   -34,   -34,     0,
       0,   -33,     0,     0,     0,     0,   -34,     0,     0,   -29,
       0,   -29,     0,     0,   -32,   -29,   -32,   -29,   -29,     0,
     -32,     0,   -32,   -32,     0,     0,     0,   -33,     0,   -33,
       0,     0,   -34,   -33,   -34,   -33,   -33,     0,   -34,     0,
     -34,   -34,   -28,   -28,     0,     0,     0,   -30,   -30,     0,
       0,     0,     0,   -28,   -28,     0,     0,     0,   -30,   -30,
     -31,   -31,   -28,     0,     0,   -75,   166,   -30,     0,     0,
       0,   -31,   -31,     0,     0,     0,   -75,   -75,     0,     0,
     -31,     0,     0,     0,     0,   -75,     0,     0,   -28,     0,
     -28,     0,     0,   -30,   -28,   -30,   -28,   -28,     0,   -30,
       0,   -30,   -30,     0,     0,     0,   -31,     0,   -31,     0,
       0,   -75,   -31,   -75,   -31,   -31,     0,   -75,     0,   -75,
     -75,   -39,   -39,     0,     0,     0,   -76,   -76,     0,     0,
       0,     0,   -39,   -39,     0,     0,     0,   -76,   -76,   -36,
     -36,   -39,     0,     0,   -37,   -37,   -76,     0,     0,     0,
     -36,   -36,     0,     0,     0,   -37,   -37,     0,     0,   -36,
       0,     0,     0,     0,   -37,     0,     0,   -39,     0,   -39,
       0,     0,   -76,   -39,   -76,   -39,   -39,     0,   -76,     0,
     -76,   -76,     0,     0,     0,   -36,     0,   -36,     0,     0,
     -37,   -36,   -37,   -36,   -36,     0,   -37,     0,   -37,   -37,
     -35,   -35,     0,     0,     0,   -38,   -38,     0,   -77,   205,
       0,   -35,   -35,     0,     0,     0,   -38,   -38,     0,     0,
     -35,     0,    84,    85,    86,   -38,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     0,   206,  -112,
       0,     0,     0,     0,     0,     0,   -35,     0,   -35,     0,
    -112,   -38,   -35,   -38,   -35,   -35,     0,   -38,  -112,   -38,
     -38,     0,     0,     0,     0,     0,     0,    84,    85,    86,
       0,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,     0,  -113,     0,     0,  -112,     0,     0,     0,
    -112,     0,  -112,  -112,  -113,     0,     0,     0,     0,     0,
       0,     0,  -113,     0,     0,     0,     0,     0,     0,     0,
       0,    84,    85,    86,     0,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,     0,   -74,     0,     0,
    -113,     0,     0,     0,  -113,     0,  -113,  -113,   -74,     0,
       0,     0,     0,     0,     0,   111,   -74,    35,    36,    37,
      38,    39,     0,     0,     0,    84,    85,    86,     0,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
       0,   -57,     0,     0,     0,     0,    40,    41,    42,     0,
     -74,   -74,   -57,   -73,     0,    35,    36,    37,    38,    39,
     -57,    43,     0,     0,   -73,     0,     0,     0,     0,    84,
      85,    86,   -73,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    40,    41,    42,     0,     0,     0,
       0,     0,     0,     0,   -57,   -57,     0,     0,     0,    43,
       0,    57,     0,    58,     0,     0,   -73,   -73,     2,     3,
       0,     0,  -119,    59,    60,     0,     0,    57,    61,    58,
    -119,    62,    63,     0,     2,     3,     0,     0,  -118,    59,
      60,     0,     0,     0,    61,     0,  -118,    62,    63,    57,
       0,    58,     0,     0,     0,     0,     2,     3,     0,     0,
    -118,    59,    60,     0,     5,     6,    61,     0,     0,    62,
      63,     0,     0,   -77,   205,    35,    36,    37,    38,    39,
       5,     6,    35,    36,    37,    38,    39,    84,    85,    86,
       0,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,     5,     6,    40,    41,    42,     0,     0,   158,
       0,    40,    41,    42,     0,     0,     0,     0,     0,    43,
    -110,     0,     0,    84,    85,    86,    43,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,   164,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    84,    85,
      86,     0,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    84,    85,    86,     0,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,     0,     0,
       0,    84,    85,    86,   175,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,     0,     0,     0,    84,
      85,    86,   179,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     0,     0,     0,     0,   128
  };

  const short
  Parser::yycheck_[] =
  {
      30,    29,    27,    51,    12,    11,    31,     0,     3,     3,
      48,     0,    20,    51,    52,    40,    41,    42,    43,    15,
       3,     3,    15,     0,     0,     0,    44,    23,     0,     1,
      23,    49,     8,     8,    59,    53,    61,    11,    63,     0,
      70,    23,    47,     0,     1,    51,     0,     1,    12,    25,
      25,     8,     9,    10,    48,   103,    20,    82,     0,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    70,    51,    51,    15,    49,    12,
     105,    15,    54,    55,     0,     1,    48,     0,     1,   119,
      66,    66,     8,     9,    10,   123,   121,    54,    55,     1,
      54,    55,   150,     0,     1,    17,     1,   103,     3,    19,
      12,   107,   104,   109,    11,   111,    54,    55,    20,   147,
      54,    55,     3,   119,   100,   100,   156,   157,   158,   159,
     160,    48,   162,   163,   164,    52,   161,    48,    54,    55,
     165,    54,    55,   171,    54,    55,   122,   122,    12,    46,
      52,   179,    54,    55,   150,    48,    20,    54,    55,   151,
       0,     1,    12,     0,     1,   195,     0,     1,    48,    48,
      20,   196,    52,    52,    11,    12,   152,   152,   208,    48,
     210,   206,   212,    20,     0,     1,    15,   217,     0,     1,
     182,    15,   184,   185,   186,   187,   188,   227,   190,   191,
     192,   156,   157,   158,   159,   160,    48,   162,   163,   164,
      52,    12,     0,     1,    54,    55,    15,    54,    55,    20,
      54,    55,   214,    15,    12,    12,   218,   219,   220,    23,
      48,   223,    20,    20,    52,   125,   103,   229,    54,    55,
     195,   150,    54,    55,     0,     1,     1,    70,     3,     0,
       1,   119,   106,   208,    62,   210,    12,   212,   216,    15,
      16,    12,   217,    19,    20,    -1,    54,    55,    -1,    20,
      -1,    -1,   227,    29,    30,    31,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      -1,    47,    48,    49,    50,    -1,    52,    53,    54,    55,
       0,     1,    -1,    54,    55,     0,     1,     0,     1,     0,
       1,    -1,    12,    -1,    -1,    15,    16,    12,    44,    19,
      20,    47,    -1,    49,    -1,    20,    -1,    53,    -1,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    -1,    47,    48,    49,
      50,    -1,    52,    53,    54,    55,     0,     1,    -1,    54,
      55,    54,    55,    54,    55,    -1,     1,    -1,    12,     1,
      -1,    15,    16,     8,    -1,    19,    20,    12,    -1,    -1,
      12,    -1,    -1,    -1,    -1,    29,    30,    31,    20,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    47,    48,    49,    50,    -1,    52,    53,
      54,    55,     0,     1,    -1,    -1,    -1,    -1,     1,    54,
      55,    -1,    54,    55,    12,     1,    -1,    15,    16,    12,
      -1,    19,    20,    -1,    -1,    -1,    12,    20,    -1,    -1,
      -1,    29,    30,    31,    20,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    45,    -1,    47,
      48,    49,    50,    -1,    52,    53,    54,    55,     0,     1,
      -1,    54,    55,    -1,    -1,    -1,    -1,    -1,    54,    55,
      12,     0,     1,    15,    16,     0,     1,    19,    20,    -1,
      -1,    -1,    11,    -1,    -1,    -1,    11,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,     0,     1,    48,    49,    50,    -1,
      52,    53,    54,    55,    -1,    -1,    12,    46,    -1,    15,
      16,    46,    51,    19,    20,    54,    55,    -1,    -1,    54,
      55,    -1,    -1,    29,    30,    31,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
       0,     1,    48,    49,    50,    -1,    52,    53,    54,    55,
       1,    -1,    12,     0,     1,    15,    16,     8,    -1,    19,
      20,    12,    -1,    -1,    11,    -1,    -1,    -1,    -1,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,     0,     1,    48,    49,
      50,    -1,    52,    53,    54,    55,    -1,    -1,    12,    46,
       1,    15,    16,    54,    55,    19,    20,    54,    55,    -1,
      -1,    12,    -1,    -1,    -1,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    45,     0,     1,    48,    49,    50,    -1,    52,    53,
      54,    55,    -1,    -1,    12,    -1,    -1,    15,    16,     1,
      -1,    19,    20,    54,    55,    -1,    -1,    -1,    -1,    -1,
      12,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    45,     0,     1,
      48,     1,    50,    -1,    52,    -1,    54,    55,     1,    -1,
      12,    -1,    12,    15,    16,     1,    -1,    19,    20,    12,
      20,    -1,    54,    55,    -1,    -1,    12,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    45,     0,     1,    48,     1,    50,    -1,
      52,    -1,    54,    55,    54,    55,    12,    -1,    12,    15,
      16,    54,    55,    19,    20,    -1,    20,    -1,    54,    55,
      -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    45,
       0,     1,    48,     1,    50,    -1,    52,    -1,    54,    55,
      54,    55,    12,    -1,    12,    15,    16,     1,    -1,    19,
      20,    -1,    20,    -1,    -1,    -1,    -1,    -1,    12,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    45,     0,     1,    48,     1,
      50,    -1,    52,    -1,    54,    55,    54,    55,    12,    -1,
      12,    15,    16,     1,    -1,    19,    20,    -1,    20,    -1,
      54,    55,    -1,    -1,    12,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    45,     0,     1,    48,     1,    50,    -1,    52,    -1,
      54,    55,    54,    55,    12,    -1,    12,    15,    16,    -1,
      -1,    19,    20,    -1,    20,    -1,    54,    55,    -1,    -1,
      -1,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    45,     0,     1,
      48,     1,    50,    -1,    52,    -1,    54,    55,    54,    55,
      12,    -1,    12,    15,    16,    -1,    -1,    19,    20,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    45,     0,     1,    48,     1,    50,    -1,
      52,    -1,    54,    55,    54,    55,    12,    -1,    12,    15,
      16,    -1,    -1,    19,    20,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    45,
       0,     1,    48,     1,    50,    -1,    52,    -1,    54,    55,
      54,    55,    12,    -1,    12,    15,    16,    -1,    -1,    19,
      20,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    45,     0,     1,    48,     1,
      50,    -1,    52,    -1,    54,    55,    54,    55,    12,    -1,
      12,    15,    16,    -1,    -1,    19,    20,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    45,     0,     1,    48,     1,    50,    -1,    52,    -1,
      54,    55,    54,    55,    12,    -1,    12,    15,    16,    -1,
      -1,    19,    20,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    45,     0,     1,
      48,     1,    50,    -1,    52,    -1,    54,    55,    54,    55,
      12,    -1,    12,    15,    16,    -1,    -1,    19,    20,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    45,     0,     1,    48,     1,    50,    -1,
      52,    -1,    54,    55,    54,    55,    12,    -1,    12,    15,
      16,    -1,    -1,    19,    20,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    45,
       0,     1,    48,     1,    50,    -1,    52,    -1,    54,    55,
      54,    55,    12,    -1,    12,    15,    16,    -1,    -1,    19,
      20,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    45,     0,     1,    48,     1,
      50,    -1,    52,    -1,    54,    55,    54,    55,    12,    -1,
      12,    15,    16,    -1,    -1,    19,    20,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    45,     0,     1,    48,     1,    50,    -1,    52,    -1,
      54,    55,    54,    55,    12,    -1,    12,    15,    16,    -1,
      -1,    19,    20,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    45,     0,     1,
      48,     1,    50,    -1,    52,    -1,    54,    55,    54,    55,
      12,    -1,    12,    15,    16,    -1,    -1,    19,    20,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    45,     0,     1,    48,    -1,    50,    -1,
      52,    -1,    54,    55,    54,    55,    12,     1,    -1,    15,
      16,    -1,    -1,    19,    20,    -1,    -1,    -1,    12,    -1,
      -1,     0,     1,    29,    30,    31,    20,    33,    34,    35,
      36,    37,    -1,    12,     1,    -1,    15,    16,    -1,    45,
      19,    20,    48,    -1,    50,    12,    52,    -1,    54,    55,
      29,    30,    31,    20,    33,    34,    35,    36,    37,    -1,
      54,    55,     0,     1,    -1,    -1,    45,    -1,    -1,    48,
      -1,    50,    -1,    52,    12,    54,    55,    15,    16,    -1,
      -1,    19,    20,    -1,    -1,    -1,    -1,    54,    55,     0,
       1,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      -1,    12,    -1,    -1,    15,    16,    -1,    45,    19,    20,
      48,    -1,    50,    -1,    52,    -1,    54,    55,    29,    30,
      31,    -1,    33,    34,    35,    36,    37,    -1,    -1,    -1,
       0,     1,    -1,    -1,    45,    -1,    -1,    48,    -1,    50,
      -1,    52,    12,    54,    55,    15,    16,    -1,    -1,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    -1,    12,
      -1,    -1,    15,    16,    -1,    45,    19,    20,    48,    -1,
      50,    -1,    52,    -1,    54,    55,    29,    30,    31,    -1,
      33,    34,    35,    36,    37,    -1,     0,     1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    -1,    50,    12,    52,
      -1,    54,    55,    -1,    -1,    -1,    20,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
       0,     1,    -1,    -1,    -1,    -1,    32,    33,    34,    -1,
      54,    55,    12,     1,    -1,     3,     4,     5,     6,     7,
      20,    47,    -1,    -1,    50,    -1,     3,     0,     1,    29,
      30,    31,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    32,    33,    34,    24,    25,    26,
      27,    28,    -1,    -1,    54,    55,    29,    30,    31,    47,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,     0,     1,    -1,    -1,     0,     1,    -1,     3,    -1,
      -1,    54,    55,     8,     9,    10,    -1,    12,    13,    14,
      -1,    -1,    -1,    18,    -1,    20,    21,    22,    -1,    -1,
      29,    30,    31,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    55,    -1,    -1,    54,
      55,     0,     1,    -1,     3,    -1,    -1,    -1,    -1,     8,
       9,    10,    -1,    12,    13,    14,    -1,     0,     1,    18,
       3,    20,    21,    22,    -1,     8,     9,    10,    -1,    12,
      13,    14,    -1,    -1,    -1,    18,    -1,    20,    21,    22,
       0,     1,    -1,     3,    -1,    -1,    -1,    -1,     8,     9,
      10,    -1,    12,    13,    14,    54,    55,    -1,    18,    -1,
      20,    21,    22,     0,     1,    -1,    -1,    -1,     0,     1,
      -1,    54,    55,    -1,    11,    12,    -1,    -1,    -1,    11,
      12,     0,     1,    20,    -1,    -1,     0,     1,    20,    -1,
      -1,    -1,    11,    12,    54,    55,    -1,    11,    12,    -1,
      -1,    20,    -1,    -1,    -1,    -1,    20,    -1,    -1,    46,
      -1,    48,    -1,    -1,    46,    52,    48,    54,    55,    -1,
      52,    -1,    54,    55,    -1,    -1,    -1,    46,    -1,    48,
      -1,    -1,    46,    52,    48,    54,    55,    -1,    52,    -1,
      54,    55,     0,     1,    -1,    -1,    -1,     0,     1,    -1,
      -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    11,    12,
       0,     1,    20,    -1,    -1,     0,     1,    20,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    11,    12,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    20,    -1,    -1,    46,    -1,
      48,    -1,    -1,    46,    52,    48,    54,    55,    -1,    52,
      -1,    54,    55,    -1,    -1,    -1,    46,    -1,    48,    -1,
      -1,    46,    52,    48,    54,    55,    -1,    52,    -1,    54,
      55,     0,     1,    -1,    -1,    -1,     0,     1,    -1,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    11,    12,     0,
       1,    20,    -1,    -1,     0,     1,    20,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    11,    12,    -1,    -1,    20,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    46,    -1,    48,
      -1,    -1,    46,    52,    48,    54,    55,    -1,    52,    -1,
      54,    55,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,
      46,    52,    48,    54,    55,    -1,    52,    -1,    54,    55,
       0,     1,    -1,    -1,    -1,     0,     1,    -1,    15,    16,
      -1,    11,    12,    -1,    -1,    -1,    11,    12,    -1,    -1,
      20,    -1,    29,    30,    31,    20,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    45,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,
      12,    46,    52,    48,    54,    55,    -1,    52,    20,    54,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,     1,    -1,    -1,    48,    -1,    -1,    -1,
      52,    -1,    54,    55,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,     1,    -1,    -1,
      48,    -1,    -1,    -1,    52,    -1,    54,    55,    12,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    20,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,     1,    -1,    -1,    -1,    -1,    32,    33,    34,    -1,
      54,    55,    12,     1,    -1,     3,     4,     5,     6,     7,
      20,    47,    -1,    -1,    12,    -1,    -1,    -1,    -1,    29,
      30,    31,    20,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    32,    33,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    47,
      -1,     1,    -1,     3,    -1,    -1,    54,    55,     8,     9,
      -1,    -1,    12,    13,    14,    -1,    -1,     1,    18,     3,
      20,    21,    22,    -1,     8,     9,    -1,    -1,    12,    13,
      14,    -1,    -1,    -1,    18,    -1,    20,    21,    22,     1,
      -1,     3,    -1,    -1,    -1,    -1,     8,     9,    -1,    -1,
      12,    13,    14,    -1,    54,    55,    18,    -1,    -1,    21,
      22,    -1,    -1,    15,    16,     3,     4,     5,     6,     7,
      54,    55,     3,     4,     5,     6,     7,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    54,    55,    32,    33,    34,    -1,    -1,    15,
      -1,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    47,
      48,    -1,    -1,    29,    30,    31,    47,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    29,    30,    31,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    29,    30,    31,    50,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    29,
      30,    31,    50,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    -1,    48
  };

  const signed char
  Parser::yystos_[] =
  {
       0,     1,     8,     9,    10,    54,    55,    60,    61,    62,
      63,    68,    69,    76,    89,    90,    91,     3,     3,     1,
       3,     0,     1,    90,    91,    61,    89,    11,    51,    11,
      11,    46,    47,    62,    91,     3,     4,     5,     6,     7,
      32,    33,    34,    47,    83,    86,     3,    24,    25,    26,
      27,    28,    70,    71,    72,    73,    70,     1,     3,    13,
      14,    18,    21,    22,    68,    77,    78,    79,    80,    83,
      90,    91,    86,     3,    64,    65,    66,    47,    86,    86,
      86,    86,    49,    53,    29,    30,    31,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    49,     1,
      74,    75,    76,    90,    91,    11,    47,     1,    86,     1,
       3,     1,    86,    86,    88,    86,    12,    81,     1,    90,
      91,    44,    78,    51,    48,    52,    87,    88,    48,    86,
       3,    23,    85,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    50,    86,     1,
      90,    91,    74,    81,    86,    87,    15,    89,    15,    15,
      89,    17,    19,    89,    19,    52,     1,    79,    86,    91,
      70,    51,    67,    66,    48,    50,    47,    84,    70,    50,
      75,    81,    91,    48,    77,    77,    77,    77,    77,    86,
      77,    77,    77,    86,    70,    11,    46,    48,    70,    81,
      81,    81,    81,    81,    81,    16,    45,    82,    20,    81,
      20,    81,    20,    81,    77,    86,    86,    15,    77,    77,
      77,    81,    82,    77,    81,    81,    81,    15,    81,    77,
      81
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    59,    60,    60,    60,    61,    61,    61,    62,    62,
      62,    63,    63,    63,    63,    63,    63,    64,    64,    65,
      65,    66,    66,    67,    67,    68,    68,    69,    70,    70,
      70,    70,    71,    71,    71,    72,    72,    73,    73,    73,
      74,    74,    74,    75,    75,    76,    76,    76,    77,    77,
      77,    78,    78,    78,    79,    79,    79,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    81,    81,    82,    82,    83,
      83,    83,    84,    84,    85,    85,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      87,    87,    88,    88,    89,    89,    90,    90,    91,    91
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     2,     3,     1,     3,     2,     1,     1,
       1,     9,     8,     6,     5,     4,     2,     0,     1,     1,
       3,     3,     1,     0,     2,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     5,     4,     4,     5,     3,
       1,     3,     2,     1,     1,     6,     4,     4,     1,     2,
       3,     1,     3,     2,     1,     1,     1,     3,     4,     2,
       5,     7,     5,    10,     8,     5,     7,     5,     7,     5,
       5,     5,     5,     1,     2,     1,     2,     0,     1,     1,
       4,     4,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     2,     4,
       0,     1,     1,     3,     1,     1,     1,     2,     0,     1
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
  "RBRACKET", "COLON", "COMMA", "DOT", "SEMICOLON", "NEWLINE", "BLOCK_END",
  "UMINUS", "UPLUS", "$accept", "program", "top_items", "top_item",
  "routine_declaration", "parameter_list", "parameters", "parameter",
  "optional_return_type", "declaration", "type_declaration", "type_spec",
  "primitive_type", "array_type", "record_type", "record_fields",
  "record_field", "variable_declaration", "body", "body_items",
  "body_item", "statement", "block_end", "optional_reverse",
  "modifiable_primary", "opt_empty_parens", "field_name", "expression",
  "argument_list", "expression_list", "sep", "seps", "opt_seps", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  Parser::yyrline_[] =
  {
       0,    91,    91,    95,    99,   106,   112,   118,   126,   130,
     134,   142,   146,   150,   154,   158,   162,   169,   173,   180,
     186,   195,   199,   206,   210,   217,   221,   228,   235,   239,
     243,   247,   254,   255,   256,   260,   264,   271,   275,   279,
     286,   292,   298,   306,   310,   318,   322,   326,   333,   337,
     341,   348,   354,   360,   368,   372,   376,   384,   388,   392,
     396,   400,   404,   408,   412,   416,   420,   424,   428,   432,
     436,   440,   444,   448,   452,   459,   460,   467,   471,   478,
     482,   486,   493,   494,   498,   499,   503,   507,   511,   515,
     519,   523,   527,   531,   535,   539,   543,   547,   551,   555,
     559,   563,   567,   571,   575,   579,   583,   587,   591,   595,
     602,   606,   613,   619,   628,   629,   633,   634,   638,   639
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
#line 3067 "src/parser/fallback/Parser.cpp"

#line 642 "src/parser/Parser.yy"

