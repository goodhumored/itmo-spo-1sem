/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
void yyerror(const char *s);

ASTNode *root = NULL;

#line 84 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_STR_LITERAL = 4,                /* STR_LITERAL  */
  YYSYMBOL_HEX_LITERAL = 5,                /* HEX_LITERAL  */
  YYSYMBOL_BITS_LITERAL = 6,               /* BITS_LITERAL  */
  YYSYMBOL_DEC_LITERAL = 7,                /* DEC_LITERAL  */
  YYSYMBOL_BOOL_LITERAL = 8,               /* BOOL_LITERAL  */
  YYSYMBOL_CHAR_LITERAL = 9,               /* CHAR_LITERAL  */
  YYSYMBOL_BOOL_TYPE = 10,                 /* BOOL_TYPE  */
  YYSYMBOL_BYTE_TYPE = 11,                 /* BYTE_TYPE  */
  YYSYMBOL_INT_TYPE = 12,                  /* INT_TYPE  */
  YYSYMBOL_UINT_TYPE = 13,                 /* UINT_TYPE  */
  YYSYMBOL_LONG_TYPE = 14,                 /* LONG_TYPE  */
  YYSYMBOL_ULONG_TYPE = 15,                /* ULONG_TYPE  */
  YYSYMBOL_CHAR_TYPE = 16,                 /* CHAR_TYPE  */
  YYSYMBOL_STRING_TYPE = 17,               /* STRING_TYPE  */
  YYSYMBOL_IF = 18,                        /* IF  */
  YYSYMBOL_ELSE = 19,                      /* ELSE  */
  YYSYMBOL_WHILE = 20,                     /* WHILE  */
  YYSYMBOL_DO = 21,                        /* DO  */
  YYSYMBOL_BREAK = 22,                     /* BREAK  */
  YYSYMBOL_23_ = 23,                       /* '='  */
  YYSYMBOL_24_ = 24,                       /* '|'  */
  YYSYMBOL_25_ = 25,                       /* '^'  */
  YYSYMBOL_26_ = 26,                       /* '&'  */
  YYSYMBOL_27_ = 27,                       /* '<'  */
  YYSYMBOL_28_ = 28,                       /* '>'  */
  YYSYMBOL_29_ = 29,                       /* '+'  */
  YYSYMBOL_30_ = 30,                       /* '-'  */
  YYSYMBOL_31_ = 31,                       /* '*'  */
  YYSYMBOL_32_ = 32,                       /* '/'  */
  YYSYMBOL_33_ = 33,                       /* '%'  */
  YYSYMBOL_34_ = 34,                       /* '!'  */
  YYSYMBOL_35_ = 35,                       /* '~'  */
  YYSYMBOL_UNARY = 36,                     /* UNARY  */
  YYSYMBOL_37_ = 37,                       /* '('  */
  YYSYMBOL_38_ = 38,                       /* '['  */
  YYSYMBOL_THEN = 39,                      /* THEN  */
  YYSYMBOL_40_ = 40,                       /* ';'  */
  YYSYMBOL_41_ = 41,                       /* ')'  */
  YYSYMBOL_42_ = 42,                       /* ','  */
  YYSYMBOL_43_ = 43,                       /* ']'  */
  YYSYMBOL_44_ = 44,                       /* '{'  */
  YYSYMBOL_45_ = 45,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 46,                  /* $accept  */
  YYSYMBOL_source = 47,                    /* source  */
  YYSYMBOL_sourceItem = 48,                /* sourceItem  */
  YYSYMBOL_funcDef = 49,                   /* funcDef  */
  YYSYMBOL_funcSignature = 50,             /* funcSignature  */
  YYSYMBOL_argDefList = 51,                /* argDefList  */
  YYSYMBOL_argDef = 52,                    /* argDef  */
  YYSYMBOL_typeRef = 53,                   /* typeRef  */
  YYSYMBOL_builtinType = 54,               /* builtinType  */
  YYSYMBOL_commaList = 55,                 /* commaList  */
  YYSYMBOL_statement = 56,                 /* statement  */
  YYSYMBOL_varDecl = 57,                   /* varDecl  */
  YYSYMBOL_varList = 58,                   /* varList  */
  YYSYMBOL_varItem = 59,                   /* varItem  */
  YYSYMBOL_ifStmt = 60,                    /* ifStmt  */
  YYSYMBOL_block = 61,                     /* block  */
  YYSYMBOL_statementList = 62,             /* statementList  */
  YYSYMBOL_whileStmt = 63,                 /* whileStmt  */
  YYSYMBOL_doStmt = 64,                    /* doStmt  */
  YYSYMBOL_breakStmt = 65,                 /* breakStmt  */
  YYSYMBOL_exprStmt = 66,                  /* exprStmt  */
  YYSYMBOL_expr = 67,                      /* expr  */
  YYSYMBOL_exprList = 68,                  /* exprList  */
  YYSYMBOL_literal = 69                    /* literal  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   318

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  76
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  130

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   279


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,     2,     2,    33,    26,     2,
      37,    41,    31,    29,    42,    30,     2,    32,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    40,
      27,    23,    28,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    38,     2,    43,    25,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    44,    24,    45,    35,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    36,    39
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    51,    51,    52,    56,    60,    64,    72,    77,    84,
      85,    86,    90,    94,    98,    99,   100,   108,   109,   110,
     111,   112,   113,   114,   115,   119,   120,   131,   132,   133,
     134,   135,   136,   137,   141,   149,   150,   154,   155,   162,
     167,   176,   180,   181,   185,   193,   201,   205,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   230,   235,   236,   240,   241,
     242,   246,   247,   248,   249,   250,   251
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "STR_LITERAL", "HEX_LITERAL", "BITS_LITERAL", "DEC_LITERAL",
  "BOOL_LITERAL", "CHAR_LITERAL", "BOOL_TYPE", "BYTE_TYPE", "INT_TYPE",
  "UINT_TYPE", "LONG_TYPE", "ULONG_TYPE", "CHAR_TYPE", "STRING_TYPE", "IF",
  "ELSE", "WHILE", "DO", "BREAK", "'='", "'|'", "'^'", "'&'", "'<'", "'>'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "UNARY", "'('", "'['",
  "THEN", "';'", "')'", "','", "']'", "'{'", "'}'", "$accept", "source",
  "sourceItem", "funcDef", "funcSignature", "argDefList", "argDef",
  "typeRef", "builtinType", "commaList", "statement", "varDecl", "varList",
  "varItem", "ifStmt", "block", "statementList", "whileStmt", "doStmt",
  "breakStmt", "exprStmt", "expr", "exprList", "literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-109)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-16)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -109,   246,  -109,   -28,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,    49,     9,  -109,    13,  -109,  -109,
    -109,   -26,  -109,    10,   -22,  -109,    11,    53,    13,   -11,
    -109,    13,  -109,    10,  -109,  -109,  -109,  -109,  -109,  -109,
     -20,    17,    40,    15,   138,   138,   138,   138,   138,  -109,
      12,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,   210,
    -109,    50,  -109,  -109,  -109,   138,   138,    74,  -109,  -109,
      14,    14,    14,    14,   133,    92,   -32,  -109,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,  -109,  -109,   153,   172,    90,  -109,   138,  -109,   129,
     241,   255,   268,   280,    48,    48,    91,    91,    14,    14,
      14,   241,    54,    77,    96,    96,   138,   241,  -109,  -109,
     138,  -109,   115,  -109,   191,   241,    96,    95,  -109,  -109
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    15,    17,    18,    19,    20,    21,    22,
      23,    24,     3,     4,     0,     0,    14,     9,     5,    42,
       6,     0,    25,    13,     0,    10,     0,     0,     9,     0,
       8,     0,    12,    66,    72,    74,    75,    76,    71,    73,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
       0,    43,    27,    28,    29,    30,    31,    32,    33,     0,
      67,     0,    26,    16,    11,     0,     0,     0,    46,    66,
      62,    61,    59,    60,     0,    37,     0,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    68,
      68,    47,     7,     0,     0,     0,    63,     0,    34,     0,
      48,    57,    58,    56,    54,    55,    49,    50,    51,    52,
      53,    69,     0,     0,     0,     0,     0,    38,    36,    64,
       0,    65,    39,    44,     0,    70,     0,     0,    40,    45
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -109,  -109,  -109,  -109,  -109,   108,   107,   120,  -109,  -109,
    -108,  -109,  -109,    51,  -109,    -9,  -109,  -109,  -109,  -109,
    -109,   -44,    59,  -109
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,    12,    13,    14,    24,    25,    50,    16,    29,
      51,    52,    76,    77,    53,    54,    27,    55,    56,    57,
      58,    59,   112,    60
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      70,    71,    72,    73,    74,    20,   122,   123,    98,    17,
      99,    28,    21,   -15,    32,    75,    23,    65,   128,    30,
      31,    93,    94,     4,     5,     6,     7,     8,     9,    10,
      11,    62,    63,    67,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   111,    22,   -15,    22,
      22,    89,    90,   117,    66,    68,    33,    34,    35,    36,
      37,    38,    39,     4,     5,     6,     7,     8,     9,    10,
      11,    40,   124,    41,    42,    43,   125,    84,    85,    86,
      87,    88,    44,    45,    19,    89,    90,    46,    47,    18,
      48,    92,    31,    19,    95,   119,   120,    19,    49,    33,
      34,    35,    36,    37,    38,    39,     4,     5,     6,     7,
       8,     9,    10,    11,    40,    97,    41,    42,    43,   120,
     121,    15,    86,    87,    88,    44,    45,   116,    89,    90,
      46,    47,    75,    48,   126,   129,    61,    26,    64,     0,
      19,    69,    34,    35,    36,    37,    38,    39,    26,   113,
     118,    26,     0,     0,     0,     0,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    44,    45,     0,
      89,    90,    46,    47,    96,    48,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,     0,     0,     0,
      89,    90,     0,     0,   114,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,     0,     0,     0,    89,
      90,     0,     0,   115,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,     0,     0,     0,    89,    90,
       0,     0,   127,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,     0,     0,     2,    89,    90,     3,
      91,     0,     0,     0,     0,     0,     4,     5,     6,     7,
       8,     9,    10,    11,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,     0,     0,     0,    89,    90,
      80,    81,    82,    83,    84,    85,    86,    87,    88,     0,
       0,     0,    89,    90,    81,    82,    83,    84,    85,    86,
      87,    88,     0,     0,     0,    89,    90,    82,    83,    84,
      85,    86,    87,    88,     0,     0,     0,    89,    90
};

static const yytype_int8 yycheck[] =
{
      44,    45,    46,    47,    48,    14,   114,   115,    40,    37,
      42,    37,     3,     3,     3,     3,     3,    37,   126,    41,
      42,    65,    66,    10,    11,    12,    13,    14,    15,    16,
      17,    42,    43,    42,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    38,    38,    38,
      38,    37,    38,    97,    37,    40,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,   116,    20,    21,    22,   120,    29,    30,    31,
      32,    33,    29,    30,    44,    37,    38,    34,    35,    40,
      37,    41,    42,    44,    20,    41,    42,    44,    45,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    23,    20,    21,    22,    42,
      43,     1,    31,    32,    33,    29,    30,    37,    37,    38,
      34,    35,     3,    37,    19,    40,    28,    17,    31,    -1,
      44,     3,     4,     5,     6,     7,     8,     9,    28,    90,
      99,    31,    -1,    -1,    -1,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    29,    30,    -1,
      37,    38,    34,    35,    41,    37,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    -1,    -1,    -1,
      37,    38,    -1,    -1,    41,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    -1,    -1,    -1,    37,
      38,    -1,    -1,    41,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    -1,    -1,    -1,    37,    38,
      -1,    -1,    41,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    -1,    -1,     0,    37,    38,     3,
      40,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    15,    16,    17,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    -1,    -1,    -1,    37,    38,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    -1,
      -1,    -1,    37,    38,    26,    27,    28,    29,    30,    31,
      32,    33,    -1,    -1,    -1,    37,    38,    27,    28,    29,
      30,    31,    32,    33,    -1,    -1,    -1,    37,    38
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    47,     0,     3,    10,    11,    12,    13,    14,    15,
      16,    17,    48,    49,    50,    53,    54,    37,    40,    44,
      61,     3,    38,     3,    51,    52,    53,    62,    37,    55,
      41,    42,     3,     3,     4,     5,     6,     7,     8,     9,
      18,    20,    21,    22,    29,    30,    34,    35,    37,    45,
      53,    56,    57,    60,    61,    63,    64,    65,    66,    67,
      69,    51,    42,    43,    52,    37,    37,    61,    40,     3,
      67,    67,    67,    67,    67,     3,    58,    59,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    37,
      38,    40,    41,    67,    67,    20,    41,    23,    40,    42,
      67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    68,    68,    41,    41,    37,    67,    59,    41,
      42,    43,    56,    56,    67,    67,    19,    41,    56,    40
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    46,    47,    47,    48,    49,    49,    50,    50,    51,
      51,    51,    52,    52,    53,    53,    53,    54,    54,    54,
      54,    54,    54,    54,    54,    55,    55,    56,    56,    56,
      56,    56,    56,    56,    57,    58,    58,    59,    59,    60,
      60,    61,    62,    62,    63,    64,    65,    66,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    68,    68,
      68,    69,    69,    69,    69,    69,    69
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     5,     4,     0,
       1,     3,     2,     1,     1,     1,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     2,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     3,     5,
       7,     3,     0,     2,     5,     7,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     4,     4,     1,     1,     0,     1,
       3,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* source: %empty  */
#line 51 "parser.y"
                { root = create_node("source", NULL); }
#line 1260 "parser.tab.c"
    break;

  case 3: /* source: source sourceItem  */
#line 52 "parser.y"
                        { if (!root) root = create_node("source", NULL); add_child(root, (yyvsp[0].node)); }
#line 1266 "parser.tab.c"
    break;

  case 4: /* sourceItem: funcDef  */
#line 56 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 1272 "parser.tab.c"
    break;

  case 5: /* funcDef: funcSignature ';'  */
#line 60 "parser.y"
                      {
        (yyval.node) = create_node("funcDef", NULL);
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1281 "parser.tab.c"
    break;

  case 6: /* funcDef: funcSignature block  */
#line 64 "parser.y"
                          {
        (yyval.node) = create_node("funcDef", NULL);
        add_child((yyval.node), (yyvsp[-1].node));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1291 "parser.tab.c"
    break;

  case 7: /* funcSignature: typeRef IDENTIFIER '(' argDefList ')'  */
#line 72 "parser.y"
                                          {
        (yyval.node) = create_node("funcSignature", (yyvsp[-3].sval));
        add_child((yyval.node), (yyvsp[-4].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1301 "parser.tab.c"
    break;

  case 8: /* funcSignature: IDENTIFIER '(' argDefList ')'  */
#line 77 "parser.y"
                                    {
        (yyval.node) = create_node("funcSignature", (yyvsp[-3].sval));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1310 "parser.tab.c"
    break;

  case 9: /* argDefList: %empty  */
#line 84 "parser.y"
                { (yyval.node) = create_node("argDefList", NULL); }
#line 1316 "parser.tab.c"
    break;

  case 10: /* argDefList: argDef  */
#line 85 "parser.y"
             { (yyval.node) = create_node("argDefList", NULL); add_child((yyval.node), (yyvsp[0].node)); }
#line 1322 "parser.tab.c"
    break;

  case 11: /* argDefList: argDefList ',' argDef  */
#line 86 "parser.y"
                            { add_child((yyvsp[-2].node), (yyvsp[0].node)); (yyval.node) = (yyvsp[-2].node); }
#line 1328 "parser.tab.c"
    break;

  case 12: /* argDef: typeRef IDENTIFIER  */
#line 90 "parser.y"
                       {
        (yyval.node) = create_node("argDef", (yyvsp[0].sval));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1337 "parser.tab.c"
    break;

  case 13: /* argDef: IDENTIFIER  */
#line 94 "parser.y"
                 { (yyval.node) = create_node("argDef", (yyvsp[0].sval)); }
#line 1343 "parser.tab.c"
    break;

  case 14: /* typeRef: builtinType  */
#line 98 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1349 "parser.tab.c"
    break;

  case 15: /* typeRef: IDENTIFIER  */
#line 99 "parser.y"
                 { (yyval.node) = create_node("customType", (yyvsp[0].sval)); }
#line 1355 "parser.tab.c"
    break;

  case 16: /* typeRef: typeRef '[' commaList ']'  */
#line 100 "parser.y"
                                {
        (yyval.node) = create_node("arrayType", NULL);
        add_child((yyval.node), (yyvsp[-3].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1365 "parser.tab.c"
    break;

  case 17: /* builtinType: BOOL_TYPE  */
#line 108 "parser.y"
              { (yyval.node) = create_node("builtinType", "bool"); }
#line 1371 "parser.tab.c"
    break;

  case 18: /* builtinType: BYTE_TYPE  */
#line 109 "parser.y"
                { (yyval.node) = create_node("builtinType", "byte"); }
#line 1377 "parser.tab.c"
    break;

  case 19: /* builtinType: INT_TYPE  */
#line 110 "parser.y"
               { (yyval.node) = create_node("builtinType", "int"); }
#line 1383 "parser.tab.c"
    break;

  case 20: /* builtinType: UINT_TYPE  */
#line 111 "parser.y"
                { (yyval.node) = create_node("builtinType", "uint"); }
#line 1389 "parser.tab.c"
    break;

  case 21: /* builtinType: LONG_TYPE  */
#line 112 "parser.y"
                { (yyval.node) = create_node("builtinType", "long"); }
#line 1395 "parser.tab.c"
    break;

  case 22: /* builtinType: ULONG_TYPE  */
#line 113 "parser.y"
                 { (yyval.node) = create_node("builtinType", "ulong"); }
#line 1401 "parser.tab.c"
    break;

  case 23: /* builtinType: CHAR_TYPE  */
#line 114 "parser.y"
                { (yyval.node) = create_node("builtinType", "char"); }
#line 1407 "parser.tab.c"
    break;

  case 24: /* builtinType: STRING_TYPE  */
#line 115 "parser.y"
                  { (yyval.node) = create_node("builtinType", "string"); }
#line 1413 "parser.tab.c"
    break;

  case 25: /* commaList: %empty  */
#line 119 "parser.y"
                { (yyval.node) = create_node("dimensions", "0"); }
#line 1419 "parser.tab.c"
    break;

  case 26: /* commaList: commaList ','  */
#line 120 "parser.y"
                    {
        char buf[32];
        int dim = atoi((yyvsp[-1].node)->value ? (yyvsp[-1].node)->value : "0") + 1;
        sprintf(buf, "%d", dim);
        free((yyvsp[-1].node)->value);
        (yyvsp[-1].node)->value = strdup(buf);
        (yyval.node) = (yyvsp[-1].node);
    }
#line 1432 "parser.tab.c"
    break;

  case 27: /* statement: varDecl  */
#line 131 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 1438 "parser.tab.c"
    break;

  case 28: /* statement: ifStmt  */
#line 132 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1444 "parser.tab.c"
    break;

  case 29: /* statement: block  */
#line 133 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 1450 "parser.tab.c"
    break;

  case 30: /* statement: whileStmt  */
#line 134 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1456 "parser.tab.c"
    break;

  case 31: /* statement: doStmt  */
#line 135 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1462 "parser.tab.c"
    break;

  case 32: /* statement: breakStmt  */
#line 136 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1468 "parser.tab.c"
    break;

  case 33: /* statement: exprStmt  */
#line 137 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1474 "parser.tab.c"
    break;

  case 34: /* varDecl: typeRef varList ';'  */
#line 141 "parser.y"
                        {
        (yyval.node) = create_node("varDecl", NULL);
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1484 "parser.tab.c"
    break;

  case 35: /* varList: varItem  */
#line 149 "parser.y"
            { (yyval.node) = create_node("varList", NULL); add_child((yyval.node), (yyvsp[0].node)); }
#line 1490 "parser.tab.c"
    break;

  case 36: /* varList: varList ',' varItem  */
#line 150 "parser.y"
                          { add_child((yyvsp[-2].node), (yyvsp[0].node)); (yyval.node) = (yyvsp[-2].node); }
#line 1496 "parser.tab.c"
    break;

  case 37: /* varItem: IDENTIFIER  */
#line 154 "parser.y"
               { (yyval.node) = create_node("var", (yyvsp[0].sval)); }
#line 1502 "parser.tab.c"
    break;

  case 38: /* varItem: IDENTIFIER '=' expr  */
#line 155 "parser.y"
                          {
        (yyval.node) = create_node("varInit", (yyvsp[-2].sval));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1511 "parser.tab.c"
    break;

  case 39: /* ifStmt: IF '(' expr ')' statement  */
#line 162 "parser.y"
                                         {
        (yyval.node) = create_node("if", NULL);
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1521 "parser.tab.c"
    break;

  case 40: /* ifStmt: IF '(' expr ')' statement ELSE statement  */
#line 167 "parser.y"
                                               {
        (yyval.node) = create_node("if", NULL);
        add_child((yyval.node), (yyvsp[-4].node));
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1532 "parser.tab.c"
    break;

  case 41: /* block: '{' statementList '}'  */
#line 176 "parser.y"
                          { (yyval.node) = (yyvsp[-1].node); }
#line 1538 "parser.tab.c"
    break;

  case 42: /* statementList: %empty  */
#line 180 "parser.y"
                { (yyval.node) = create_node("block", NULL); }
#line 1544 "parser.tab.c"
    break;

  case 43: /* statementList: statementList statement  */
#line 181 "parser.y"
                              { add_child((yyvsp[-1].node), (yyvsp[0].node)); (yyval.node) = (yyvsp[-1].node); }
#line 1550 "parser.tab.c"
    break;

  case 44: /* whileStmt: WHILE '(' expr ')' statement  */
#line 185 "parser.y"
                                 {
        (yyval.node) = create_node("while", NULL);
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1560 "parser.tab.c"
    break;

  case 45: /* doStmt: DO block WHILE '(' expr ')' ';'  */
#line 193 "parser.y"
                                    {
        (yyval.node) = create_node("do", NULL);
        add_child((yyval.node), (yyvsp[-5].node));
        add_child((yyval.node), (yyvsp[-2].node));
    }
#line 1570 "parser.tab.c"
    break;

  case 46: /* breakStmt: BREAK ';'  */
#line 201 "parser.y"
              { (yyval.node) = create_node("break", NULL); }
#line 1576 "parser.tab.c"
    break;

  case 47: /* exprStmt: expr ';'  */
#line 205 "parser.y"
             { (yyval.node) = create_node("exprStmt", NULL); add_child((yyval.node), (yyvsp[-1].node)); }
#line 1582 "parser.tab.c"
    break;

  case 48: /* expr: expr '=' expr  */
#line 209 "parser.y"
                  { (yyval.node) = create_node("binary", "="); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1588 "parser.tab.c"
    break;

  case 49: /* expr: expr '+' expr  */
#line 210 "parser.y"
                    { (yyval.node) = create_node("binary", "+"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1594 "parser.tab.c"
    break;

  case 50: /* expr: expr '-' expr  */
#line 211 "parser.y"
                    { (yyval.node) = create_node("binary", "-"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1600 "parser.tab.c"
    break;

  case 51: /* expr: expr '*' expr  */
#line 212 "parser.y"
                    { (yyval.node) = create_node("binary", "*"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1606 "parser.tab.c"
    break;

  case 52: /* expr: expr '/' expr  */
#line 213 "parser.y"
                    { (yyval.node) = create_node("binary", "/"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1612 "parser.tab.c"
    break;

  case 53: /* expr: expr '%' expr  */
#line 214 "parser.y"
                    { (yyval.node) = create_node("binary", "%"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1618 "parser.tab.c"
    break;

  case 54: /* expr: expr '<' expr  */
#line 215 "parser.y"
                    { (yyval.node) = create_node("binary", "<"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1624 "parser.tab.c"
    break;

  case 55: /* expr: expr '>' expr  */
#line 216 "parser.y"
                    { (yyval.node) = create_node("binary", ">"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1630 "parser.tab.c"
    break;

  case 56: /* expr: expr '&' expr  */
#line 217 "parser.y"
                    { (yyval.node) = create_node("binary", "&"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1636 "parser.tab.c"
    break;

  case 57: /* expr: expr '|' expr  */
#line 218 "parser.y"
                    { (yyval.node) = create_node("binary", "|"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1642 "parser.tab.c"
    break;

  case 58: /* expr: expr '^' expr  */
#line 219 "parser.y"
                    { (yyval.node) = create_node("binary", "^"); add_child((yyval.node), (yyvsp[-2].node)); add_child((yyval.node), (yyvsp[0].node)); }
#line 1648 "parser.tab.c"
    break;

  case 59: /* expr: '!' expr  */
#line 220 "parser.y"
                           { (yyval.node) = create_node("unary", "!"); add_child((yyval.node), (yyvsp[0].node)); }
#line 1654 "parser.tab.c"
    break;

  case 60: /* expr: '~' expr  */
#line 221 "parser.y"
                           { (yyval.node) = create_node("unary", "~"); add_child((yyval.node), (yyvsp[0].node)); }
#line 1660 "parser.tab.c"
    break;

  case 61: /* expr: '-' expr  */
#line 222 "parser.y"
                           { (yyval.node) = create_node("unary", "-"); add_child((yyval.node), (yyvsp[0].node)); }
#line 1666 "parser.tab.c"
    break;

  case 62: /* expr: '+' expr  */
#line 223 "parser.y"
                           { (yyval.node) = create_node("unary", "+"); add_child((yyval.node), (yyvsp[0].node)); }
#line 1672 "parser.tab.c"
    break;

  case 63: /* expr: '(' expr ')'  */
#line 224 "parser.y"
                   { (yyval.node) = create_node("braces", NULL); add_child((yyval.node), (yyvsp[-1].node)); }
#line 1678 "parser.tab.c"
    break;

  case 64: /* expr: expr '(' exprList ')'  */
#line 225 "parser.y"
                            {
        (yyval.node) = create_node("call", NULL);
        add_child((yyval.node), (yyvsp[-3].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1688 "parser.tab.c"
    break;

  case 65: /* expr: expr '[' exprList ']'  */
#line 230 "parser.y"
                            {
        (yyval.node) = create_node("indexer", NULL);
        add_child((yyval.node), (yyvsp[-3].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1698 "parser.tab.c"
    break;

  case 66: /* expr: IDENTIFIER  */
#line 235 "parser.y"
                 { (yyval.node) = create_node("identifier", (yyvsp[0].sval)); }
#line 1704 "parser.tab.c"
    break;

  case 67: /* expr: literal  */
#line 236 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1710 "parser.tab.c"
    break;

  case 68: /* exprList: %empty  */
#line 240 "parser.y"
                { (yyval.node) = create_node("exprList", NULL); }
#line 1716 "parser.tab.c"
    break;

  case 69: /* exprList: expr  */
#line 241 "parser.y"
           { (yyval.node) = create_node("exprList", NULL); add_child((yyval.node), (yyvsp[0].node)); }
#line 1722 "parser.tab.c"
    break;

  case 70: /* exprList: exprList ',' expr  */
#line 242 "parser.y"
                        { add_child((yyvsp[-2].node), (yyvsp[0].node)); (yyval.node) = (yyvsp[-2].node); }
#line 1728 "parser.tab.c"
    break;

  case 71: /* literal: BOOL_LITERAL  */
#line 246 "parser.y"
                 { (yyval.node) = create_node("literal", (yyvsp[0].bval) ? "true" : "false"); }
#line 1734 "parser.tab.c"
    break;

  case 72: /* literal: STR_LITERAL  */
#line 247 "parser.y"
                  { (yyval.node) = create_node("literal", (yyvsp[0].sval)); }
#line 1740 "parser.tab.c"
    break;

  case 73: /* literal: CHAR_LITERAL  */
#line 248 "parser.y"
                   { char buf[2] = {(yyvsp[0].cval), 0}; (yyval.node) = create_node("literal", buf); }
#line 1746 "parser.tab.c"
    break;

  case 74: /* literal: HEX_LITERAL  */
#line 249 "parser.y"
                  { (yyval.node) = create_node("literal", (yyvsp[0].sval)); }
#line 1752 "parser.tab.c"
    break;

  case 75: /* literal: BITS_LITERAL  */
#line 250 "parser.y"
                   { (yyval.node) = create_node("literal", (yyvsp[0].sval)); }
#line 1758 "parser.tab.c"
    break;

  case 76: /* literal: DEC_LITERAL  */
#line 251 "parser.y"
                  { (yyval.node) = create_node("literal", (yyvsp[0].sval)); }
#line 1764 "parser.tab.c"
    break;


#line 1768 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 254 "parser.y"


void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}
