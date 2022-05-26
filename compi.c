
#include "main.h"
#include "ad.h"
#include "at.h"
#include "mv.h"
// #define SAFEALLOC(var, Type)                          \
//     if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
//         err("not enough memory");

enum
{
    ID,
    BREAK,
    CHAR,
    DOUBLE,
    ELSE,
    FOR,
    IF,
    INT,
    RETURN,
    STRUCT,
    VOID,
    WHILE,
    CT_INT,
    CT_REAL,
    CT_CHAR,
    CT_STRING,
    COMMA,
    SEMICOLON,
    LPAR,
    RPAR,
    LBRACKET,
    RBRACKET,
    LACC,
    RACC,
    END,
    ADD,
    SUB,
    MUL,
    DIV,
    DOT,
    AND,
    OR,
    NOT,
    ASSIGN,
    EQUAL,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,

}; // codurile AL

char *tokenNames[] = {"ID", "BREAK", "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END", "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "ASSIGN", "EQUAL", "NOTEQ", "LESS", "LESSEQ", "GREATER", "GREATEREQ"};
typedef struct _Token
{
    int code; // codul (numele)
    union
    {
        char *text; // folosit pentru ID, CT_STRING (alocat dinamic)
        int i;      // folosit pentru CT_INT, CT_CHAR
        double r;   // folosit pentru CT_REAL
    };
    int line;            // linia din fisierul de intrare
    struct _Token *next; // inlantuire la urmatorul AL
} Token;

int line = 1;
Token *tokens, *lastToken;
char *pCrtCh;
void err(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}
void tkerr(const Token *tk, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}
Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk, Token)
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastToken)
    {
        lastToken->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastToken = tk;
    return tk;
}
char *createString(const char *start, const char *end)
{
    char *string;
    int size;
    size = end - start;
    string = malloc(size * sizeof(char));
    strncpy(string, start, (size));
    string[size] = '\0';
    return string;
}

int getNextToken()
{
    int state = 0, nCh;
    char ch, character;
    const char *pStartCh;
    Token *tk;

    for (;;)
    {
        // bucla infinita
        ch = *pCrtCh;
        // printf("LINIE %d STARE %d CU %c %d\n", line, state, ch, ch);
        switch (state)
        {
        case 0: // testare tranzitii posibile din starea 0
            if (ch == '*')
            {
                pCrtCh++;  // consuma caracterul
                state = 1; // trece la noua stare
            }
            else if (ch == '/')
            {
                pCrtCh++;
                state = 2;
            }
            else if (ch == '+')
            {
                pCrtCh++;
                state = 5;
            }
            else if (ch == '-')
            {
                pCrtCh++;
                state = 6;
            }
            else if (ch == '&')
            {
                pCrtCh++;
                state = 8;
            }
            else if (ch == '|')
            {
                pCrtCh++;
                state = 10;
            }
            else if (ch == '!')
            {
                pCrtCh++;
                state = 12;
            }
            else if (ch == '<')
            {
                pCrtCh++;
                state = 14;
            }
            else if (ch == '=')
            {
                pCrtCh++;
                state = 17;
            }
            else if (ch == '>')
            {
                pCrtCh++;
                state = 20;
            }
            else if (ch == '.')
            {
                pCrtCh++;
                state = 7;
            }
            else if (ch == ' ' || ch == '\r' || ch == '\t')
            {
                pCrtCh++; // consuma caracterul si ramane in starea 0
            }
            else if (ch == '\n')
            {
                // tratat separat pentru a actualiza linia curenta
                line++;
                pCrtCh++;
            }
            else if (ch == '\0')
            {
                // sfarsit de sir
                pCrtCh++;
                state = 42;
            }
            else if (ch == '[')
            {
                pCrtCh++;
                state = 46;
            }
            else if (ch == ';')
            {
                pCrtCh++;
                state = 38;
            }
            else if (ch == ',')
            {
                pCrtCh++;
                state = 39;
            }
            else if (ch == '(')
            {
                pCrtCh++;
                state = 40;
            }
            else if (ch == ')')
            {
                pCrtCh++;
                state = 41;
            }
            else if (ch == '}')
            {
                pCrtCh++;
                state = 43;
            }
            else if (ch == '{')
            {
                pCrtCh++;
                state = 44;
            }
            else if (ch == ']')
            {
                pCrtCh++;
                state = 45;
            }
            else if (isdigit(ch))
            {
                pStartCh = pCrtCh;
                pCrtCh++;
                state = 23;
            }
            else if (ch == '\'')
            {
                pCrtCh++;
                state = 30;
            }
            else if (ch == '"')
            {
                pStartCh = pCrtCh; // memoram inceputul CT_STRING
                pCrtCh++;
                state = 33;
            }
            else if (isalpha(ch) || ch == '_')
            {
                pStartCh = pCrtCh;
                pCrtCh++;
                state = 35;
            }
            else
                tkerr(addTk(END), "caracter invalid");
            break;
        case 1:
            tk = addTk(MUL);
            return MUL;
        case 2:
            if (ch == '/')
            {
                pCrtCh++;
                state = 4;
            }
            else
            {
                state = 3;
            }
            break;
        case 3:
            tk = addTk(DIV);
            return DIV;
        case 4:

            if (ch != '\n' && ch != '\r' && ch != '\0')
            {
                pCrtCh++;
            }
            else if (ch == '\n')
            { // pCrtCh++;
                state = 0;
                line++;
            }
            else
                state = 0;
            break;
        case 5:
            tk = addTk(ADD);
            return ADD;
        case 6:
            tk = addTk(SUB);
            return SUB;
        case 7:
            tk = addTk(DOT);
            return DOT;
        case 8:
            if (ch == '&')
            {
                pCrtCh++;
                state = 9;
            }
            else
            {
                tkerr(addTk(AND), "trebuie 2 &");
                return 1;
            }
            break;
        case 9:
            tk = addTk(AND);
            return AND;
        case 10:
            if (ch == '|')
            {
                pCrtCh++;
                state = 11;
            }
            else
            {
                tkerr(addTk(OR), "trebuie 2 |");
                return 1;
            }
            break;
        case 11:
            tk = addTk(OR);
            return OR;
        case 12:
            if (ch == '=')
            {
                pCrtCh++;
                state = 47;
            }
            else
            {
                state = 13;
            }
            break;
        case 13:
            tk = addTk(NOT);
            return NOT;
        case 14:
            if (ch == '=')
            {
                state = 16;
                pCrtCh++;
            }
            else
            {
                state = 15;
            }
            break;
        case 15:
            tk = addTk(LESS);
            return LESS;
        case 16:
            tk = addTk(LESSEQ);
            return LESSEQ;
        case 17:
            if (ch == '=')
            {
                pCrtCh++;
                state = 19;
            }
            else
                state = 18;
            break;
        case 18:
            tk = addTk(ASSIGN);
            return ASSIGN;
        case 19:
            tk = addTk(EQUAL);
            return EQUAL;
        case 20:
            if (ch == '=')
            {
                pCrtCh++;
                state = 22;
            }
            else
            {
                state = 21;
            }
            break;
        case 21:
            tk = addTk(GREATER);
            return (GREATER);
        case 22:
            tk = addTk(GREATEREQ);
            return GREATEREQ;
        case 23:
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else if (ch == 'e' || ch == 'E')
            {
                pCrtCh++;
                state = 26;
            }
            else if (ch == '.')
            {
                pCrtCh++;
                state = 24;
            }
            else
                state = 37;
            break;
        case 24:
            if (isdigit(ch))
            {
                pCrtCh++;
                state = 25;
            }
            else
            {
                tkerr(addTk(CT_REAL), "dupa . trebuie sa urmeze cifra");
            }
            break;
        case 25:
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else if (ch == 'e' || ch == 'E')
            {
                pCrtCh++;
                state = 26;
            }
            else
                state = 29;
            break;
        case 26:
            if (ch == '+' || ch == '-')
            {
                pCrtCh++;
            }
            else
                state = 27;
            break;
        case 27:
            if (isdigit(ch))
            {
                pCrtCh++;
                state = 28;
            }
            break;
        case 28:
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else
                state = 29;
            break;
        case 29:
            tk = addTk(CT_REAL);
            tk->r = atof(createString(pStartCh, pCrtCh));
            return CT_REAL;
        case 30:
            if (ch != '\'')
            {
                pCrtCh++;
                character = ch;
                // state = 30;
            }
            else
                state = 31;
            break;
        case 31:
            if (ch == '\'')
            {
                pCrtCh++;
                state = 32;
            }
            break;
        case 32:
            tk = addTk(CT_CHAR);
            tk->i = character;
            return CT_CHAR;
        case 33:
            if (ch != '"')
            {
                pCrtCh++;
            }
            else
            {
                pCrtCh++;
                state = 34;
            }
            break;
        case 34:
            tk = addTk(CT_STRING);
            tk->text = createString(pStartCh, pCrtCh);
            // printf("%s", tk->text);
            return CT_STRING; // RETURN CT_STRING
                              // break;
        case 35:
            if (isalnum(ch) || ch == '_')
            { // printf("ch=%c",ch);
                pCrtCh++;
                // state=35;
            }
            else
                state = 36;
            break;
        case 36:
            nCh = pCrtCh - pStartCh;

            if (nCh == 5 && !memcmp(pStartCh, "break", 5))
                tk = addTk(BREAK);
            else if (nCh == 4 && !memcmp(pStartCh, "char", 4))
                tk = addTk(CHAR);
            else if (nCh == 6 && !memcmp(pStartCh, "double", 6))
                tk = addTk(DOUBLE);
            else if (nCh == 4 && !memcmp(pStartCh, "else", 4))
                tk = addTk(ELSE);
            else if (nCh == 3 && !memcmp(pStartCh, "for", 3))
                tk = addTk(FOR);
            else if (nCh == 2 && !memcmp(pStartCh, "if", 2))
                tk = addTk(IF);
            else if (nCh == 3 && !memcmp(pStartCh, "int", 3))
                tk = addTk(INT);
            else if (nCh == 6 && !memcmp(pStartCh, "return", 6))
                tk = addTk(RETURN);
            else if (nCh == 6 && !memcmp(pStartCh, "struct", 4))
                tk = addTk(STRUCT);
            else if (nCh == 4 && !memcmp(pStartCh, "void", 4))
                tk = addTk(VOID);
            else if (nCh == 5 && !memcmp(pStartCh, "while", 5))
                tk = addTk(WHILE);
            else
            {
                tk = addTk(ID);
                tk->text = createString(pStartCh, pCrtCh);
            }
            return tk->code;

        case 37:
            tk = addTk(CT_INT);
            tk->i = atoi(createString(pStartCh, pCrtCh));
            // printf("%d",tk->i);
            return CT_INT;
        case 38:
            addTk(SEMICOLON);
            return SEMICOLON;
        case 39:
            tk = addTk(COMMA);
            return COMMA;
        case 40:
            tk = addTk(LPAR);
            return LPAR;
        case 41:
            tk = addTk(RPAR);
            return RPAR;
        case 42:
            addTk(END);
            return END;
        case 43:
            tk = addTk(RACC);
            return RACC;
        case 44:
            tk = addTk(LACC);
            return LACC;
        case 45:
            tk = addTk(RBRACKET);
            return RBRACKET;
        case 46:
            tk = addTk(LBRACKET);
            return LBRACKET;
        case 47:
            tk = addTk(NOTEQ);
            return NOTEQ;
        default:
            err("eroare la token");
            return 0;
        }
    }
}

void showAtoms(Token *tk)
{

    printf("LINE : %d  ", tk->line);
    printf("%s ", tokenNames[tk->code]);
    switch (tk->code)
    {
    case CT_STRING:
        printf("%s", tk->text);
        break;
    case CT_CHAR:
        printf("%c", tk->i);
        break;
    case ID:
        printf("%s", tk->text);
        break;
    case CT_INT:
        printf("%d", tk->i);
        break;
    case CT_REAL:
        printf("%lf", tk->r);
        break;
    }
    printf("\n");
    if (tk->next != NULL)
        showAtoms(tk->next);
}

Symbol *owner = NULL;

bool unit();
bool structDef();
bool varDef();
bool typeBase(Type *t);
bool arrayDecl(Type *t);
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound(bool newDomain);
bool expr(Ret *r);
bool exprAssign(Ret *r);
bool exprOr(Ret *r);
bool exprOrPrim(Ret *r);
bool exprAnd(Ret *r);
bool exprAndPrim(Ret *r);
bool exprEq(Ret *r);
bool exprEqPrim(Ret *r);
bool exprRel(Ret *r);
bool exprRelPrim(Ret *r);

bool exprAdd(Ret *r);
bool exprAddPrim(Ret *r);
bool exprMul(Ret *r);
bool exprMulPrim(Ret *r);
bool exprCast(Ret *r);
bool exprUnary(Ret *r);

bool exprPostFix(Ret *r);
bool exprPostFixPrim(Ret *r);
bool exprPrimary();
Token *iTk;        // iteratorul în lista de atomi. Inițial pointează la primul atom din listă.
Token *consumedTk; // atomul care tocmai a fost consumat. Va fi folosit în etapele  următoare ale compilatorului.

const char *tkCodeName(int code)
{
    return tokenNames[code];
}
bool consume(int code)
{
    // printf("consume(%s)", tkCodeName(code));
    if (iTk->code == code)
    {
        consumedTk = iTk;
        iTk = iTk->next;
        // printf(" => consumed\n");
        return true;
    }
    // printf(" => found %s %d\n ", tkCodeName(iTk->code), iTk->line);
    return false;
}

bool typeBase(Type *t)
{
    // printf("typebase\n");
    Token *startTk = iTk;
    int startInstr=nInstructions;
    t->n = -1;
    if (consume(INT))
    {
        t->tb = TB_INT;
        return true;
    }
    if (consume(DOUBLE))
    {
        t->tb = TB_DOUBLE;
        return true;
    }
    if (consume(CHAR))
    {
        t->tb = TB_CHAR;
        return true;
    }
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            Token *tkName = consumedTk;
            t->tb = TB_STRUCT;
            t->s = findSymbol(tkName->text);
            if (!t->s)
                tkerr(iTk, "structura nedefinita: %s", tkName->text);
            return true;
        }
        else
        {
            tkerr(iTk, "Se asteapta ID dupa struct");
        }
    }
    nInstructions=startInstr;
    iTk = startTk;
    return false;
}
bool arrayDecl(Type *t)
{
    if (consume(LBRACKET))
    {
        if (consume(CT_INT))
        {
            Token *tkSize = consumedTk;
            t->n = tkSize->i;
        }
        else
        {
            t->n = 0; // array fara dimensiune: int v[]
        }
        if (consume(RBRACKET))
        {
            return true;
        }
        else
            tkerr(iTk, "missing ] or invalid expression inside [...]");
    }
    return false;
}

bool varDef()
{
    // printf("varDef\n");
    Type t;
    Token *startTk = iTk;
    int startInstr=nInstructions;
    if (typeBase(&t))
    {
        if (consume(ID))
        {
            Token *tkName = consumedTk;
            if (arrayDecl(&t))
            {
                if (t.n == 0)
                    tkerr(iTk, "a vector variable must have a specified dimension");
            }
            if (consume(SEMICOLON))
            {
                Symbol *var = findSymbolInDomain(symTable, tkName->text);
                if (var)
                    tkerr(iTk, "symbol redefinition: %s", tkName->text);
                var = newSymbol(tkName->text, SK_VAR);
                var->type = t;
                var->owner = owner;
                addSymbolToDomain(symTable, var);
                if (owner)
                {
                    switch (owner->kind)
                    {
                    case SK_FN:
                        var->varIdx = symbolsLen(owner->fn.locals);
                        addSymbolToList(&owner->fn.locals, dupSymbol(var));
                        break;
                    case SK_STRUCT:
                        var->varIdx = typeSize(&owner->type);
                        addSymbolToList(&owner->structMembers, dupSymbol(var));
                        break;
                    }
                }
                else
                {
                    var->varIdx = allocInGlobalMemory(typeSize(&t));
                }
                return true;
            }
            else
            {
                tkerr(iTk, "Se asteapta ; la linia %d", iTk->line);
            }
        }
        else
        {
            tkerr(iTk, "Se asteapta ID dupa un tip de baza");
        }
    }
    nInstructions=startInstr;
    iTk = startTk;
    return false;
}
bool structDef()
{
    // printf("StructDef\n");
    Token *startTk = iTk;
    int startInstr=nInstructions;
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            Token *tkName = consumedTk;
            if (consume(LACC))
            {
                Symbol *s = findSymbolInDomain(symTable, tkName->text);
                if (s)
                    tkerr(iTk, "symbol redefinition: %s", tkName->text);
                s = addSymbolToDomain(symTable, newSymbol(tkName->text, SK_STRUCT));
                s->type.tb = TB_STRUCT;
                s->type.s = s;
                s->type.n = -1;
                pushDomain();
                owner = s;

                while (1)
                {
                    if (varDef())
                    {
                    }
                    else
                        break;
                }
                // printf("aici structDef");
                if (consume(RACC))
                {
                    if (consume(SEMICOLON))
                    {
                        owner = NULL;
                        dropDomain();
                        return true;
                    }
                    else
                    {
                        tkerr(iTk, "Lipseste ; la definitia structurii");
                    }
                }
                else
                {
                    tkerr(iTk, "Lipseste } din defintia structurii");
                }
            }
        }
        else
        {
            tkerr(iTk, "Se asteapta un ID dupa struct");
        }
    }
    nInstructions=startInstr;
    iTk = startTk;
    return false;
}
bool fnParam()
{
    // printf("fnParam\n");
    // Token *startTk = iTk;
    Type t;
    if (typeBase(&t))
    {
        if (consume(ID))
        {
            Token *tkName = consumedTk;
            if (arrayDecl(&t))
            {
                t.n = 0;
            }
            Symbol *param = findSymbolInDomain(symTable, tkName->text);
            if (param)
                tkerr(iTk, "symbol redefinition: %s", tkName->text);
            param = newSymbol(tkName->text, SK_PARAM);

            param->type = t;
            param->owner=owner;
            param->paramIdx = symbolsLen(owner->fn.params);
            // parametrul este adaugat atat la domeniul curent, cat si la parametrii fn
            addSymbolToDomain(symTable, param);
            addSymbolToList(&owner->fn.params, dupSymbol(param));
            return true;
        }
        else
        {
            tkerr(iTk, "Lipseste ID din defintia functiei");
        }
    }
    // iTk = startTk;
    return false;
}

bool unit()
{
    // printf("unit\n");
    addInstr(OP_CALL);
    addInstr(OP_HALT);
    for (;;)
    { // buclă infinită
        if (structDef())
        {
        }
        else if (fnDef())
        {
        }
        else if (varDef())
        {
        }
        else
            break; // dacă nu se poate consuma nimic la iterația curentă, se iese din buclă
    }
    if (consume(END))
    { // se ajunge la END chiar și dacă nu se consumă nimic în repetiție, deci ea este opțională
        Symbol *sm = findSymbol("main");
        if (!sm)
            tkerr(iTk, "undefined: main");
        instructions[0].arg.i = sm->fn.instrIdx;
        return true;
    }
    else
    {
        tkerr(iTk, "Lipseste sfarsitul de fisier");
    }
    return false;
}

bool fnDef()
{
    Token *back = iTk;
    int startInstr=nInstructions;
    Type t;
    if (typeBase(&t))
    {
    }
    else if (consume(VOID))
    {
        t.tb = TB_VOID;
    }
    else
        return false;
    if (consume(ID))
    {
        Token *tkName = consumedTk;

        if (consume(LPAR))
        {
            Symbol *fn = findSymbolInDomain(symTable, tkName->text);
            if (fn)
                tkerr(iTk, "symbol redefinition: %s", tkName->text);
            fn = newSymbol(tkName->text, SK_FN);
            fn->type = t;
            addSymbolToDomain(symTable, fn);
            owner = fn;
            pushDomain();
            if (fnParam())
            {
                for (;;)
                {
                    if (consume(COMMA))
                    {
                        if (fnParam())
                        {
                        }
                        else
                        {
                            tkerr(iTk, "Se asteapta argumente pentru functie dupa ,");
                        }
                    }
                    else
                        break;
                }
            }
            if (consume(RPAR))
            {
                owner->fn.instrIdx = nInstructions;
                addInstr(OP_ENTER);
                if (stmCompound(false))
                {
                    dropDomain();
                    instructions[owner->fn.instrIdx].arg.i = symbolsLen(owner->fn.locals);
                    if (owner->type.tb == TB_VOID)
                        addInstrWithInt(OP_RET_VOID, symbolsLen(owner->fn.params));

                    owner = NULL;

                    return true;
                }
            }
            else
            {
                tkerr(iTk, "Lipseste ) din definitia functiei");
            }
        }
    }

    iTk = back;
    return false;
}
bool stm()
{
    Ret rInit, rCond, rStep, rExpr;
    if (stmCompound(true))
    {
        return true;
    }
    else if (consume(IF))
    {
        if (consume(LPAR))
        {
            if (expr(&rCond))
            {
                if (!canBeScalar(&rCond))
                    tkerr(iTk, "the if condition must be a scalar value");
                if (consume(RPAR))
                {
                    addRVal(rCond.lval, &rCond.type);
                    Type intType = {TB_INT, NULL, -1};
                    insertConvIfNeeded(nInstructions, &rCond.type, &intType);
                    int posJF = addInstr(OP_JF);
                    if (stm())
                    {
                        if (consume(ELSE))
                        {
                            int posJMP = addInstr(OP_JMP);
                            instructions[posJF].arg.i = nInstructions;

                            if (stm())
                            {
                                instructions[posJMP].arg.i = nInstructions;
                                return true;
                            }
                            else
                            {
                                tkerr(iTk, "Se asteapta o expresie compusa dupa else");
                            }
                        }
                        else
                        {
                            instructions[posJF].arg.i = nInstructions;
                        }
                        return true;
                    }
                    else
                    {
                        tkerr(iTk, "Se asteapta o expresie compusa dupa if");
                    }
                }
                else
                {
                    tkerr(iTk, "Lipseste ) la if");
                }
            }
            else
            {
                tkerr(iTk, "Se asteapta o expresie dupa (");
            }
        }
        else
        {
            tkerr(iTk, "Lipseste ( dupa if");
        }
    }
    else if (consume(WHILE))
    {
        int posCond = nInstructions;
        if (consume(LPAR))
        {
            if (expr(&rCond))
            {
                if (!canBeScalar(&rCond))
                    tkerr(iTk, "the while condition must be a scalar value");
                if (consume(RPAR))
                {
                    addRVal(rCond.lval, &rCond.type);
                    Type intType = {TB_INT, NULL, -1};
                    insertConvIfNeeded(nInstructions, &rCond.type, &intType);
                    int posJF = addInstr(OP_JF);

                    if (stm())
                    {
                        addInstrWithInt(OP_JMP, posCond);
                        instructions[posJF].arg.i = nInstructions;
                        return true;
                    }
                    else
                    {
                        tkerr(iTk, "Se asteapta o expresie compusa dupa while");
                    }
                }
                else
                {
                    tkerr(iTk, "Lipseste ) dupa while");
                }
            }
            else
            {
                tkerr(iTk, "Se asteapta o expresie dupa (");
            }
        }
        else
        {
            tkerr(iTk, "Lipseste ( dupa while");
        }
    }
    else if (consume(FOR))
    {
        if (consume(LPAR))
        {
            if (expr(&rInit))
            {
            }
            if (consume(SEMICOLON))
            {
                if (expr(&rCond))
                {
                    if (!canBeScalar(&rCond))
                        tkerr(iTk, "the for condition must be a scalar value");
                }
                if (consume(SEMICOLON))
                {
                    if (expr(&rStep))
                    {
                    }
                    if (consume(RPAR))
                    {
                        if (stm())
                            return true;
                    }
                    else
                    {
                        tkerr(iTk, "Lipseste ) de la for");
                    }
                }
                else
                {
                    tkerr(iTk, "Lipseste ; dupa a doua expresie din for");
                }
            }
            else
            {
                tkerr(iTk, "Lipseste ; dupa prima expresie din for");
            }
        }
        else
        {
            tkerr(iTk, "Lipseste ( dupa for");
        }
    }
    else if (consume(BREAK))
    {
        if (consume(SEMICOLON))
            return true;
        else
        {
            tkerr(iTk, "Lipseste ; dupa break");
        }
    }
    else if (consume(RETURN))
    {
        if (expr(&rExpr))
        {
            if (owner->type.tb == TB_VOID)
                tkerr(iTk, "a void function cannot return a value");
            if (!canBeScalar(&rExpr))
                tkerr(iTk, "the return value must be a scalar value");
            if (!convTo(&rExpr.type, &owner->type))
                tkerr(iTk, "cannot convert the return expression type to the function return type");
            addRVal(rExpr.lval, &rExpr.type);
            insertConvIfNeeded(nInstructions, &rExpr.type, &owner->type);
            addInstrWithInt(OP_RET, symbolsLen(owner->fn.params));
        }
        else
        {
            if (owner->type.tb != TB_VOID)
                tkerr(iTk, "a non-void function must return a value");
            addInstr(OP_RET_VOID);
        }
        if (consume(SEMICOLON))
            return true;
        else
        {
            tkerr(iTk, "Lipseste ; dupa return");
        }
    }
    else if (expr(&rExpr))
    {
        if (rExpr.type.tb != TB_VOID)
            addInstr(OP_DROP);
        if (consume(SEMICOLON))
            return true;
        else
        {
            tkerr(iTk, "Lipseste ; dupa expresie");
        }
    }
    else if (consume(SEMICOLON))
    {
        return true;
    }

    return false;
}

bool stmCompound(bool newDomain)
{
    Token *start = iTk;
    int startInstr=nInstructions;
    if (consume(LACC))
    {
        if (newDomain)
            pushDomain();
        for (;;)
        {
            if (varDef())
            {
            }
            else if (stm())
            {
            }
            else
                break;
        }
        if (consume(RACC))
        {
            if (newDomain)
                dropDomain();
            return true;
        }
        else
        {
            tkerr(iTk, "Lipseste } intr-o expresie compusa");
        }
    }
    iTk = start;
    return false;
}

bool expr(Ret *r)
{
    if (exprAssign(r))
        return true;
    return false;
}

bool exprAssign(Ret *r)
{
    Token *startTk = iTk;
    int startInstr=nInstructions;
    Ret rDst;
    if (exprUnary(&rDst))
    {
        if (consume(ASSIGN))
        {
            if (exprAssign(r))
            {
                if (!rDst.lval)
                    tkerr(iTk, "the assign destination must be a left-value");
                if (rDst.ct)
                    tkerr(iTk, "the assign destination cannot be constant");
                if (!canBeScalar(&rDst))
                    tkerr(iTk, "the assign destination must be scalar");
                if (!canBeScalar(r))
                    tkerr(iTk, "the assign source must be scalar");
                if (!convTo(&r->type, &rDst.type))
                    tkerr(iTk, "the assign source cannot be converted to destination");
                r->lval = false;
                r->ct = true;
                addRVal(r->lval, &r->type);
                insertConvIfNeeded(nInstructions, &r->type, &rDst.type);
                switch (rDst.type.tb)
                {
                case TB_INT:
                    addInstr(OP_STORE_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_STORE_F);
                    break;
                }
                return true;
            }
            else
            {
                tkerr(iTk, "Lipseste o expresie dupa =");
            }
        }
        nInstructions=startInstr;
        iTk = startTk;
    }
    if (exprOr(r))
    {
        return true;
    }
    return false;
}

bool exprOr(Ret *r)
{
    if (exprAnd(r))
    {
        if (exprOrPrim(r))
        {
            return true;
        }
    }
    return false;
}

bool exprOrPrim(Ret *r)
{
    if (consume(OR))
    {
        Ret right;
        if (exprAnd(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for ||");
            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprOrPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipste o expresie dupa ||");
        }
    }
    return true;
}

bool exprAnd(Ret *r)
{
    if (exprEq(r))
    {
        if (exprAndPrim(r))
        {
            return true;
        }
    }

    return false;
}

bool exprAndPrim(Ret *r)
{
    if (consume(AND))
    {
        Ret right;
        if (exprEq(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for &&");
            *r = (Ret){{TB_INT, NULL, -1}, false, true};

            if (exprAndPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa &&");
        }
    }
    return true;
}

bool exprEq(Ret *r)
{
    if (exprRel(r))
    {
        if (exprEqPrim(r))
        {
            return true;
        }
    }

    return false;
}

bool exprEqPrim(Ret *r)
{
    if (consume(EQUAL))
    {
        Ret right;

        if (exprRel(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for == or !=");
            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprEqPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipreste expresie dupa ==");
        }
    }
    else if (consume(NOTEQ))
    {
        Ret right;
        if (exprRel(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for == or !=");
            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprEqPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa !=");
        }
    }
    return true;
}

bool exprRel(Ret *r)
{
    if (exprAdd(r))
    {
        if (exprRelPrim(r))
        {
            return true;
        }
    }
    return false;
}

bool exprRelPrim(Ret *r)
{
    Token *op;
    if (consume(LESS))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprAdd(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for <, <=, >, >=");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case LESS:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_LESS_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_LESS_F);
                    break;
                }
                break;
            }

            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprRelPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa <");
        }
    }
    else if (consume(LESSEQ))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprAdd(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for <, <=, >, >=");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case LESS:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_LESS_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_LESS_F);
                    break;
                }
                break;
            }

            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprRelPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa <=");
        }
    }
    else if (consume(GREATER))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprAdd(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for <, <=, >, >=");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case LESS:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_LESS_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_LESS_F);
                    break;
                }
                break;
            }

            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprRelPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa >");
        }
    }
    else if (consume(GREATEREQ))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprAdd(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for <, <=, >, >=");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case LESS:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_LESS_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_LESS_F);
                    break;
                }
                break;
            }

            *r = (Ret){{TB_INT, NULL, -1}, false, true};
            if (exprRelPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa >=");
        }
    }
    return true;
}

bool exprAdd(Ret *r)
{
    if (exprMul(r))
    {
        if (exprAddPrim(r))
        {
            return true;
        }
    }
    return false;
}

bool exprAddPrim(Ret *r)
{
    Token *op;
    if (consume(ADD))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprMul(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for + or -");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case ADD:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_ADD_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_ADD_F);
                    break;
                }
                break;
            case SUB:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_SUB_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_SUB_F);
                    break;
                }
                break;
            }

            *r = (Ret){tDst, false, true};
            if (exprAddPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa +");
        }
    }
    else if (consume(SUB))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprMul(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for + or -");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case ADD:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_ADD_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_ADD_F);
                    break;
                }
                break;
            case SUB:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_SUB_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_SUB_F);
                    break;
                }
                break;
            }

            *r = (Ret){tDst, false, true};
            if (exprAddPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa -");
        }
    }
    return true;
}

bool exprMul(Ret *r)
{
    if (exprCast(r))
    {
        if (exprMulPrim(r))
        {
            return true;
        }
    }
    return false;
}

bool exprMulPrim(Ret *r)
{
    Token *op;
    if (consume(MUL))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprCast(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for * or /");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case MUL:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_MUL_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_MUL_F);
                    break;
                }
                break;
            case DIV:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_DIV_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_DIV_F);
                    break;
                }
                break;
            }

            *r = (Ret){tDst, false, true};
            if (exprMulPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa *");
        }
    }
    else if (consume(DIV))
    {
        op = consumedTk;
        Ret right;
        int posLeft = nInstructions;
        addRVal(r->lval, &r->type);
        if (exprCast(&right))
        {
            Type tDst;
            if (!arithTypeTo(&r->type, &right.type, &tDst))
                tkerr(iTk, "invalid operand type for * or /");
            addRVal(right.lval, &right.type);
            insertConvIfNeeded(posLeft, &r->type, &tDst);
            insertConvIfNeeded(nInstructions, &right.type, &tDst);
            switch (op->code)
            {
            case MUL:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_MUL_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_MUL_F);
                    break;
                }
                break;
            case DIV:
                switch (tDst.tb)
                {
                case TB_INT:
                    addInstr(OP_DIV_I);
                    break;
                case TB_DOUBLE:
                    addInstr(OP_DIV_F);
                    break;
                }
                break;
            }

            *r = (Ret){tDst, false, true};
            if (exprMulPrim(r))
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa /");
        }
    }
    return true;
}

bool exprCast(Ret *r)
{
    Token *startTk = iTk;
    int startInstr=nInstructions;
    Type t;
    if (consume(LPAR))
    {
        Type t;
        Ret op;
        if (typeBase(&t))
        {
            if (arrayDecl(&t))
            {
            }
            if (consume(RPAR))
            {
                if (exprCast(&op))
                {
                    if (t.tb == TB_STRUCT)
                        tkerr(iTk, "cannot convert to a struct type");
                    if (op.type.tb == TB_STRUCT)
                        tkerr(iTk, "cannot convert a struct");
                    if (op.type.n >= 0 && t.n < 0)
                        tkerr(iTk, "an array can be converted only to another array");
                    if (op.type.n < 0 && t.n >= 0)
                        tkerr(iTk, "a scalar can be converted only to another scalar");
                    *r = (Ret){t, false, true};
                    return true;
                }
            }
        }
        nInstructions=startInstr;
        iTk = startTk;
    }
    if (exprUnary(r))
    {
        return true;
    }
    else
        return false;
}

bool exprUnary(Ret *r)
{
    if (consume(SUB))
    {
        if (exprUnary(r))
        {
            if (!canBeScalar(r))
                tkerr(iTk, "unary - must have a scalar operand");
            r->lval = false;
            r->ct = true;
            return true;
        }
    }
    else if (consume(NOT))
    {
        if (exprUnary(r))
        {
            if (!canBeScalar(r))
                tkerr(iTk, "unary - must have a scalar operand");
            r->lval = false;
            r->ct = true;
            return true;
        }
    }
    else if (exprPostFix(r))
    {
        return true;
    }

    return false;
}

bool exprPostFix(Ret *r)
{
    if (exprPrimary(r))
    {
        if (exprPostFixPrim(r))
        {
            return true;
        }
    }
    return false;
}

bool exprPostFixPrim(Ret *r)
{
    if (consume(LBRACKET))
    {
        Ret idx;
        if (expr(&idx))
        {
            if (consume(RBRACKET))
            {
                if (r->type.n < 0)
                    tkerr(iTk, "only an array can be indexed");
                Type tInt = {TB_INT, NULL, -1};
                if (!convTo(&idx.type, &tInt))
                    tkerr(iTk, "the index is not convertible to int");
                r->type.n = -1;
                r->lval = true;
                r->ct = false;
                if (exprPostFixPrim(r))
                {
                    return true;
                }
            }
            else
            {
                tkerr(iTk, "Lipseste ] dupa [");
            }
        }
        else
        {
            tkerr(iTk, "Lipseste expresie dupa [");
        }
    }
    else if (consume(DOT))
    {
        if (consume(ID))
        {
            Token *tkName = consumedTk;
            if (r->type.tb != TB_STRUCT)
                tkerr(iTk, "a field can only be selected from a struct");
            Symbol *s = findSymbolInList(r->type.s->structMembers, tkName->text);
            if (!s)
                tkerr(iTk, "the structure %s does not have a field %s", r->type.s->name, tkName->text);
            *r = (Ret){s->type, true, s->type.n >= 0};
            if (exprPostFixPrim(r))
            {
                return true;
            }
        }
    }
    return true;
}

bool exprPrimary(Ret *r)
{
    Token *startTk = iTk;
    int startInstr=nInstructions;
    if (consume(ID))
    {
        Token *tkName = consumedTk;
        Symbol *s = findSymbol(tkName->text);
        if (!s)
            tkerr(iTk, "undefined id: %s", tkName->text);
        if (consume(LPAR))
        {
            if (s->kind != SK_FN)
                tkerr(iTk, "only a function can be called");
            Ret rArg;
            Symbol *param = s->fn.params;
            if (expr(&rArg))
            {
                if (!param)
                    tkerr(iTk, "too many arguments in function call");
                if (!convTo(&rArg.type, &param->type))
                    tkerr(iTk, "in call, cannot convert the argument type to the parameter type");
                addRVal(rArg.lval, &rArg.type);
                insertConvIfNeeded(nInstructions, &rArg.type, &param->type);
                param = param->next;
                for (;;)
                {
                    if (consume(COMMA))
                    {
                        if (expr(&rArg))
                        {
                            if (!param)
                                tkerr(iTk, "too many arguments in function call");
                            if (!convTo(&rArg.type, &param->type))
                                tkerr(iTk, "in call, cannot convert the argument type to the parameter type");
                            addRVal(rArg.lval, &rArg.type);
                            insertConvIfNeeded(nInstructions, &rArg.type, &param->type);

                            param = param->next;
                        }
                        else
                        {

                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            if (consume(RPAR))
            {
                if (param)
                    tkerr(iTk, "too few arguments in function call");
                *r = (Ret){s->type, false, true};
                if (s->fn.extFnPtr)
                {
                    int posCallExt = addInstr(OP_CALL_EXT);
                    instructions[posCallExt].arg.extFnPtr = s->fn.extFnPtr;
                }
                else
                {
                    addInstrWithInt(OP_CALL, s->fn.instrIdx);
                }
                return true;
            }
            else
            {
                tkerr(iTk, "Lipseste ) dupa (");
            }
        }
        else
        {
            if (s->kind == SK_FN)
                tkerr(iTk, "a function can only be called");
            *r = (Ret){s->type, true, s->type.n >= 0};
            if (s->kind == SK_VAR)
            {
                if (s->owner == NULL)
                { // variabile globale
                    addInstrWithInt(OP_ADDR, s->varIdx);
                }
                else
                { // variabile locale
                    switch (s->type.tb)
                    {
                    case TB_INT:
                        addInstrWithInt(OP_FPADDR_I, s->varIdx + 1);
                        break;
                    case TB_DOUBLE:
                        addInstrWithInt(OP_FPADDR_F, s->varIdx + 1);
                        break;
                    }
                }
            }
            if (s->kind == SK_PARAM)
            {
                switch (s->type.tb)
                {
                case TB_INT:
                    addInstrWithInt(OP_FPADDR_I, s->paramIdx - symbolsLen(s->owner->fn.params) - 1);
                    break;
                case TB_DOUBLE:
                    addInstrWithInt(OP_FPADDR_F, s->paramIdx - symbolsLen(s->owner->fn.params) - 1);
                    break;
                }
            }
        }
        return true;
    }
    if (consume(CT_INT))
    {
        Token *ct = consumedTk;
        addInstrWithInt(OP_PUSH_I, ct->i);
        *r = (Ret){{TB_INT, NULL, -1}, false, true};
        return true;
    }
    if (consume(CT_REAL))
    {
        Token *ct = consumedTk;
        addInstrWithDouble(OP_PUSH_F, ct->r);
        *r = (Ret){{TB_DOUBLE, NULL, -1}, false, true};
        return true;
    }
    if (consume(CT_CHAR))
    {
        Token *ct = consumedTk;
        *r = (Ret){{TB_CHAR, NULL, -1}, false, true};
        return true;
    }
    if (consume(CT_STRING))
    {
        Token *ct = consumedTk;
        *r = (Ret){{TB_CHAR, NULL, 0}, false, true};
        return true;
    }
    if (consume(LPAR))
    {

        if (expr(r))
        {
            if (consume(RPAR))
            {
                return true;
            }
            else
            {
                tkerr(iTk, "Lipseste ) ");
            }
        }
    }
    nInstructions=startInstr;
    iTk = startTk;
    return false;
}

char inbuf[3000];
int main()
{
    FILE *fis = fopen("test.c", "rb");
    if (fis == NULL)
    {
        printf("eroare: nu s-a putut deschide test.c\n");
        return 1;
    }
    while (!feof(fis))
    {
        fread(inbuf, sizeof(inbuf), 1, fis);
    }

    fclose(fis);

    pCrtCh = inbuf;

    while (getNextToken() != END)
    {
    }
    //  Token *tk = tokens;
    // showAtoms(tk);
    iTk = tokens;
    pushDomain();
    mvInit();
    if (unit())
    {
        printf("Sintaxa corecta\n");
    }
    else
    {
    }
    showDomain(symTable, "global");
    // genTestProgram1();
    run();
    dropDomain();
    return 0;
}