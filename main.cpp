#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstring>


 
enum type_of_lex { 
    LEX_NULL,                                                                                   
    LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_ELSE,  LEX_IF, LEX_FALSE, LEX_INT,  LEX_STRING,           
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ,  LEX_TRUE, LEX_WHILE, LEX_CONTINUE, LEX_WRITE,           
    LEX_FIN,                                                                                    
    LEX_SEMICOLON, LEX_COMMA,LEX_EQ, LEX_LPAREN, LEX_RPAREN, LEX_ASSIGN, LEX_LSS,              
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,  LEX_LEFTBRACE, LEX_RIGHTBRACE,  LEX_QUOTE,    
    LEX_NUM, LEX_STR,  LEX_UNARY_MINUS, LEX_UNARY_PLUS, LEX_WRITE_START,                                                                    
    LEX_ID,                                                                                     
    POLIZ_LABEL,                                                                               
    POLIZ_ADDRESS,                                                                              
    POLIZ_GO,                                                                                  
    POLIZ_FGO                                                                                   
};
 
class Lex {
    type_of_lex   t_lex;
    int           v_lex;
    std::string   s_lex;
public:

    Lex ( type_of_lex t = LEX_NULL): t_lex (t) { v_lex=0; s_lex="";}
    Lex ( type_of_lex t, int v ): t_lex (t), v_lex (v)  { }
    Lex ( type_of_lex t, std::string v): t_lex (t), s_lex (v)  { }

    type_of_lex  get_type () const { 
    	  return t_lex; 
    }
    int get_value () const { 
    	  return v_lex; 
    }
    std::string get_str () const { 
    	  return s_lex; 
    }
    Lex& operator=(const Lex& obj1);
  friend std::ostream & operator<< (std::ostream &s, Lex l );
};

    Lex& Lex::operator=(const Lex& obj1){
        t_lex = obj1.t_lex;
        v_lex= obj1.v_lex;
        s_lex= obj1.s_lex;
        return *this;
    }
 
int lines=1;
class Ident {
    std::string      name;
    bool             declare;
    type_of_lex      type;
    bool             assign;
    int              value;
    std::string      str_value;  //save str value of ident 
public:
    Ident() { 
        declare = false; 
        assign  = false; 
    }
    bool operator== ( const std::string& s ) const { 
        return name == s; 
    }
    Ident ( const std::string n ) {
        name    = n;
        declare = false; 
        assign  = false;
    }
    std::string get_name () const { 
      return name; 
    }
    bool get_declare () const { 
      return declare; 
    }
    void put_declare () { 
      declare   = true; 
    }
    type_of_lex get_type () const { 
      return type; 
    }
    void put_type ( type_of_lex t ) { 
      type      = t; 
    }
    bool get_assign () const { 
      return assign; 
    }
    void put_assign () { 
      assign    = true; 
    }
    int  get_value () const { 
      return value; 
    }
    void put_value ( int v ) { 
      value = v; 
    }
    std::string get_str_value () const { 
      return str_value; 
    }
    void put_str_value (const std::string& v) { 
      str_value  = v; 
    }
};
 
std::vector<Ident> TID;
 
int put ( const std::string & buf ) {
    std::vector<Ident>::iterator k;
 
    if ( ( k = find ( TID.begin(), TID.end(), buf)) != TID.end () )
        return k - TID.begin();
    TID.push_back ( Ident(buf) );
    return TID.size () - 1;
}
 
class Scanner {
    FILE * fp;
    char   c;
   
    int look ( const std::string buf, const char ** list ) {
        int i = 0;
        while ( list[i] ) {
            if ( buf == list[i] )
                return i;
            ++i;
        }
        return 0;
    }
    void gc () {
        c = fgetc (fp);
    }
public:
    static const char * TW [], * TD [];
    Scanner ( const char * program ) {
        if ( !(fp = fopen ( program, "r" )) ) 
            throw  "can’t open file" ;
    }
     ~Scanner() {
        fclose(fp);
    }
    Lex get_lex ();
};
 
const char *
Scanner::TW    [] = { "",  "and", "begin", "boolean",  "else", "if", "false", "int", "string", "not", "or", "program", 
                        "read",  "true", "while", "continue", "write", NULL };
 
const char *
Scanner::TD    [] = { "0",";", ",",  "==", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", 
                         "{", "}", "\"",  NULL };
 
Lex Scanner::get_lex () {
    enum    state { H, IDENT, NUMB, COM, ALE, NEQ, STR};
    int     d, j;
    std::string  buf;
    state   CS = H;  //start symbol
    do {
        gc ();
        switch ( CS ) {
            case H:
                if ( c==' ' || c == '\n' || c== '\r' || c == '\t' ) {if ( c == '\n' ) lines++; }   
                    else if ( isalpha (c) ) {
                        buf.push_back (c);
                        CS  = IDENT;
                    }
                    else if ( isdigit (c) ) {
                        d   = c - '0';
                        CS  = NUMB;
                    } 
                    else if ( c== '{' ) { 
                        buf.push_back (c);
                        j = look ( buf, TD);
                        return Lex(LEX_LEFTBRACE,j);
                    }
                    else if ( c== '}' ) { 
                        buf.push_back (c);
                        j = look (buf, TD);
                       return Lex(LEX_RIGHTBRACE,j);
                    }
                    else if ( c == '=' || c == '<' || c == '>' ) { 
                        buf.push_back (c);
                        CS  = ALE; 
                    }
                    else if (c == EOF)
                        return Lex ( LEX_FIN );
                    else if (c == '!') {
                        buf.push_back (c);
                        CS  = NEQ;
                    }
                    else if (c=='/'){
                        gc();
                        if (c=='*'){
                            CS= COM;
                        }
                        else {
                            ungetc(c, fp);
                            c= '/';
                            {
                                buf.push_back (c);
                                if ( ( j = look ( buf, TD) ) ){
                                        return Lex ( (type_of_lex)( (j) + (int) LEX_FIN ), j );
                                    }
                                else  throw c;
                            }
                        }
                    }
                    else if (c == '"'){
                        CS=STR;
                    }
                    else {
                        buf.push_back (c);
                       
                        if ( ( j = look ( buf, TD) ) ){
                            return Lex ( (type_of_lex)( (j) + (int) LEX_FIN ), j );
                        }
                        else
                        
                            throw c;
                    }
                    break;
            case IDENT:
                if ( isalpha (c) || isdigit (c) ) {
                    buf.push_back (c); 
                }
                else {
                    ungetc ( c, fp );
                    if ( (j = look ( buf, TW) ) ) {
                        return Lex ( (type_of_lex) j, j );
                        }
                        else {
                        j   = put ( buf );
                        return Lex ( LEX_ID, j );
                    }
                }
                break;
            case NUMB:
                if ( isdigit (c) ) {
                    d = d * 10 + ( c - '0' );
                }
                else {
                    ungetc ( c, fp );
                    return Lex ( LEX_NUM, d );
                }
                break;
            case COM:
                if ( c == '*' ) {
                    gc();
                    if (c== '/') {
                        CS= H;
                    }

                }
                else if (c == EOF)
                    throw "unexpected end of file";
                break;
            case ALE:
                if ( c == '=' ) {
                    buf.push_back ( c );
                    j   = look ( buf, TD );
                    return Lex ( (type_of_lex) ( j + (int) LEX_FIN ), j );
                }
                else {
                    ungetc ( c, fp );
                    j   = look ( buf, TD );
                    return Lex ( (type_of_lex) ( j + (int) LEX_FIN ), j );
                }
                break;
            case STR:
                buf.clear();
                while ( c!= '"'){
                   
                    if (c==EOF)
                        throw "Unexpected EOF";
                    buf.push_back(c);
                    gc();
                }
                return Lex(LEX_STR, buf);
                break;
            case NEQ:
                if ( c == '=' ) {
                    buf.push_back(c);
                    j   = look ( buf, TD );
                    return Lex ( LEX_NEQ, j );
                }
                else
                    throw '!';
                break;
    } 
  } while (true);
}
 
std::ostream & operator<< (std::ostream &s, Lex l ) {
    std::string t;
    if ( l.t_lex <= LEX_WRITE )
        t = Scanner::TW[l.t_lex];
    else if ( l.t_lex >= LEX_FIN && l.t_lex <= LEX_QUOTE )
        t = Scanner::TD[ l.t_lex - LEX_FIN ];
     else if ( l.t_lex == LEX_UNARY_MINUS)
        t = "-";
    else if ( l.t_lex == LEX_UNARY_PLUS)
        t = "+";
    else if ( l.t_lex == LEX_NUM )
        t = "NUMB";
    else if ( l.t_lex == LEX_STR )
        t = "STR";
    else if ( l.t_lex == LEX_ID )
        t = TID[l.v_lex].get_name ();
    else if ( l.t_lex == POLIZ_LABEL )
        t = "Label";
    else if ( l.t_lex == POLIZ_ADDRESS )
        t = "Addr";
    else if ( l.t_lex == POLIZ_GO )
        t = "!";
    else if ( l.t_lex == POLIZ_FGO ) 
        t = "!F";
    else if ( l.t_lex == LEX_WRITE_START ) 
        t = "WRITE START";
    else{
        throw l;
    }
    if ( l.t_lex == LEX_STR ) s << '(' << t << ',' << l.s_lex << ");" << std::endl;
    else s  << '(' << t << ',' << l.v_lex << ");" << std::endl;
    return s;
}
 
template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) {
    i = st.top(); st.pop();
}
 
class Parser {
    Lex          curr_lex, next_lex, prev_lex;
    type_of_lex  c_type;
    int          c_val;
    Scanner      scan;
    int loop =0;
    int countskob=0;
    bool flag;
    std::stack < int >           st_int;
    std::stack < type_of_lex >   st_lex;
    std::stack<int> loop_start;             // Стек для хранения начала каждого цикла
    bool flag_prev=false, flag_next=false;
    void  P();    //program
    void  D1();   // {opisaniya}
    void  D(type_of_lex cc_type);      // opisaniye
    void  B();      //block operatorov
    void  S();      // operator
    void  A();      //prisvaivanie
    void  E();     
    void  E1();
    void  T();
    void  T1();
    void  T2();
    void  F();
    void  dec ( type_of_lex type);
    void  check_id (int=-1);
    void  check_op ();
    void  check_not ();
    void  eq_type ();
    void  eq_bool ();
    void check_id_in_read ();
    void check_unary();
    void proc_continue();
    void gl() {
        if (flag_prev) {
            next_lex=curr_lex;
            curr_lex=prev_lex;
            flag_prev=false;
            flag_next=true;
        }
        else if (flag_next) {
            prev_lex=curr_lex;
            curr_lex=next_lex;
            flag_next=false;
        }
        else {
            prev_lex=curr_lex;
            curr_lex=scan.get_lex();
        }
        
        c_type=curr_lex.get_type(); 
        c_val=curr_lex.get_value();
          
    }

public:
    std::vector <Lex> poliz;
    Parser ( const char *program ) : scan (program) { }
    void  analyze();
};
 

void Parser::analyze () {
    gl();
    P();
    if (c_type != LEX_FIN)
        throw curr_lex;
    //for_each( std::vector <Lex> :: iterator p = poliz.begin(); p != poliz.end(); p++) { std::cout << *p;};
    int i = 0;
    // for ( Lex l : poliz ) {
    //     std::cout << i<< "    "<<l;
    //     i++;}
    std::cout << std::endl << "Yes!!! Yes!!! Your program is correct!!" << std::endl;
}

 
void Parser::P () {
    if (c_type == LEX_PROGRAM ) {
        gl();
    }
    else 
        throw curr_lex;   
    if (c_type == LEX_LEFTBRACE){
        gl();
    }   
    else
        throw curr_lex;
    
      
    D1 ();   //описания

    
    B ();   // blok operatorov
    if (c_type == LEX_RIGHTBRACE)
        gl ();
    else
        throw curr_lex;
}
 
void Parser::D1 () {
    while ( c_type == LEX_INT || c_type == LEX_BOOL || c_type== LEX_STRING) {
        type_of_lex id_type=c_type;
        D(id_type);
        dec(id_type);
    }
    
}
 
void Parser::D (type_of_lex cc_type) {
    gl();
    do {
        if (c_type!= LEX_ID){ 
            throw curr_lex; //("Syntax error: EXpected identifier");
        }

        //poliz.push_back ( Lex ( LEX_ID, c_val ) );
        st_int.push(c_val);
        gl();
        if (c_type == LEX_ASSIGN){
            dec(cc_type);
            check_id(prev_lex.get_value());
            poliz.push_back ( Lex (POLIZ_ADDRESS, prev_lex.get_value() ) );
            gl();
            E();   //expression
            eq_type();
            poliz.push_back ( Lex ( LEX_ASSIGN ));
             while (st_lex.size()!=0){
                st_lex.pop();
            } 
        }

        if (c_type!= LEX_COMMA && c_type != LEX_SEMICOLON){
            throw curr_lex; //("Syntax error: expected , or ;")
        }

        if (c_type == LEX_COMMA){
            gl();
        }
    } while (c_type!= LEX_SEMICOLON);
    gl();
}

 
void Parser::B () {
       
        S();
        while (st_lex.size()!=0){
                st_lex.pop();
            } 

        while ( c_type == LEX_SEMICOLON || (countskob !=0 && c_type == LEX_RIGHTBRACE) ) {
            if (countskob!=0 && c_type == LEX_RIGHTBRACE) {countskob--;}
            gl ();
            S ();
            while (st_lex.size()!=0)
                st_lex.pop();
            }
        
}
 
void Parser::S () {
    
     while (st_lex.size()!=0){
                st_lex.pop();
            } 
    int pl0, pl1, pl2, pl3;
      if (c_type == LEX_ID) {
        A();
       if ( c_type != LEX_SEMICOLON ) {
            throw curr_lex;  
        }
    }
    else if ( c_type == LEX_IF ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            E();
            eq_bool ();
            pl2 = poliz.size();
            poliz.push_back ( Lex() );
            poliz.push_back ( Lex(POLIZ_FGO) );
        if ( c_type == LEX_RPAREN ) {
            gl ();
        } else throw curr_lex;
        }
        else throw curr_lex;
        S();
        pl3 = poliz.size ();
        poliz.push_back ( Lex () );
 
        poliz.push_back ( Lex ( POLIZ_GO ) );
        poliz[pl2] = Lex ( POLIZ_LABEL, poliz.size() );
       
        gl();
        if (countskob!=0 && c_type == LEX_ELSE ) countskob--;
        
        if ( c_type == LEX_ELSE ) {
            gl ();
            S ();
               
            poliz[pl3] = Lex ( POLIZ_LABEL, poliz.size() );
        }
        else {
            poliz[pl3] = Lex ( POLIZ_LABEL, poliz.size() );
            flag_prev=true;
            gl();
        }
           
    }//end if

    else if ( c_type == LEX_WHILE ) {
        loop++;
        // int loop_begin = poliz.size();  // Получаем позицию начала цикла
        // loop_start.push(loop_begin);    // Сохраняем в стек
        loop_start.push(poliz.size());
        pl0 = poliz.size ();
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            E();
            eq_bool ();
            pl1 = poliz.size (); 
            poliz.push_back ( Lex () );
            poliz.push_back ( Lex (POLIZ_FGO) );
        if ( c_type == LEX_RPAREN ) {
            gl ();
        } else throw curr_lex;
        }
        else throw curr_lex;
        S();
        poliz.push_back ( Lex ( POLIZ_LABEL, pl0 ) );
        poliz.push_back ( Lex ( POLIZ_GO) );
        poliz[pl1] = Lex ( POLIZ_LABEL, poliz.size() );
        loop--;
        loop_start.pop();
        }//end while

    else if ( c_type == LEX_READ ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            if ( c_type == LEX_ID ) {
                check_id_in_read ();
                poliz.push_back ( Lex( POLIZ_ADDRESS, c_val) );
                 gl();
            }
            else throw curr_lex;
        if ( c_type == LEX_RPAREN ) {
            gl ();
            poliz.push_back ( Lex (LEX_READ) );
        } else throw curr_lex;

        if ( c_type != LEX_SEMICOLON )  throw curr_lex;
        }
        else throw curr_lex;
    }//end read

    else if ( c_type == LEX_WRITE ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            poliz.push_back(Lex(LEX_WRITE_START));
            gl ();
            E ();
        while (c_type == LEX_COMMA){
            gl();
            E();
        }
        if ( c_type == LEX_RPAREN ) {
            gl ();
             poliz.push_back ( Lex ( LEX_WRITE ) );}
        else throw curr_lex;
        if ( c_type != LEX_SEMICOLON ) throw curr_lex;

        } else throw curr_lex;
    }//end write

    else if ( c_type == LEX_LEFTBRACE ) {
            countskob++;
            gl();
            S();
          while ( c_type == LEX_SEMICOLON ) {
            gl ();
            S ();
        }
         if ( c_type == LEX_RIGHTBRACE) {
            //gl ();
            //S();
         }
         else {
            throw curr_lex;
         }
    }

    else if (c_type == LEX_NUM || c_type == LEX_STR || c_type == LEX_MINUS || c_type == LEX_TRUE || c_type == LEX_FALSE ||
                c_type == LEX_PLUS || c_type == LEX_LPAREN){
        A();
        if ( c_type != LEX_SEMICOLON ) throw curr_lex;
    }
    else if (c_type == LEX_CONTINUE){
        proc_continue();
        gl();
        if ( c_type != LEX_SEMICOLON ) throw curr_lex;
    }
}

void Parser::A(){                   //=
    if (c_type == LEX_ID){
        //check_id ();  
        //Lex saved_lex= curr_lex;
        gl();

        if (c_type == LEX_ASSIGN){
            check_id(prev_lex.get_value());
            poliz.push_back (Lex ( POLIZ_ADDRESS,  prev_lex.get_value()) );
            gl();
            //eq_type();
            A();
            eq_type();
            poliz.push_back ( Lex ( LEX_ASSIGN ) );

        } else{
            //check_id();
            flag_prev=true;
            gl();
            E();
            //eq_type();
            //poliz.push_back ( Lex ( LEX_ASSIGN ) );     
        }
      
    } else{
        E();
    }
    
}

void Parser::E () {                //or
    E1 ();
    while ( c_type == LEX_OR ) {
        st_lex.push ( c_type );
        gl (); 
        E1 (); 
        check_op ();
    }
}
 
void Parser::E1 () {               //and
    T ();
    while ( c_type == LEX_AND) {
        st_lex.push ( c_type );
        gl ();
        T ();
        check_op ();
    }
}
 
void Parser::T () {                //othosheniya
    T1();
    while ( c_type == LEX_EQ  || c_type == LEX_LSS || c_type == LEX_GTR ||
         c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ ){
        st_lex.push ( c_type );
        gl ();
        T1();
        check_op ();
    }
}

void Parser::T1 () {                //+-
    T2();
    while ( c_type == LEX_MINUS  || c_type == LEX_PLUS ){
        st_lex.push ( c_type );     
        gl ();
        T2();
        check_op ();
    }
}

void Parser::T2 () {                // * /
    F();
    while ( c_type == LEX_SLASH  || c_type == LEX_TIMES ){
        st_lex.push ( c_type );
        gl ();
        F();
        check_op ();
    }
}
 

void Parser::F() {
    switch (c_type) {
        case LEX_ID: 
            check_id();
            poliz.push_back ( Lex ( LEX_ID, c_val ) );
            gl(); 
            break;

        case LEX_NUM:
            st_lex.push ( LEX_INT );
            poliz.push_back ( curr_lex );
            gl(); 
            break;

        case LEX_STR:
            st_lex.push ( LEX_STRING );
            poliz.push_back ( curr_lex );
            gl(); 
            break;

        
        case LEX_FALSE:
            st_lex.push ( LEX_BOOL );
            poliz.push_back ( Lex (LEX_FALSE, 0) );
            gl(); 
            break;

        case LEX_TRUE:
            st_lex.push ( LEX_BOOL );
            poliz.push_back ( Lex (LEX_TRUE, 1) );
            gl(); 
            break;

        case LEX_NOT:
            gl(); 
            F(); 
            check_not ();
            break;
        
        
         case LEX_PLUS:
            if (prev_lex.get_type()==LEX_PLUS || prev_lex.get_type()==LEX_MINUS )  throw curr_lex;// Обработка унарного плюса
            st_lex.push(LEX_PLUS);
            gl();
            F(); 
            check_unary();
             poliz.push_back(Lex(LEX_UNARY_PLUS));
            break;

        case LEX_MINUS: // Обработка унарного минуса
         if (prev_lex.get_type()==LEX_PLUS || prev_lex.get_type()==LEX_MINUS )  throw curr_lex;
            st_lex.push(LEX_MINUS);
            gl();
            F(); 
            check_unary();
             poliz.push_back(Lex(LEX_UNARY_MINUS));
            break;
            
        case LEX_LPAREN:
            gl();    
            A();  
            if (c_type != LEX_RPAREN) {
                throw curr_lex;
            }
            gl(); 
            break;

        default:
            throw curr_lex;
    }
}

void Parser::proc_continue() {
    // if (loop == 0) {
    //     throw "continue outside of loop"; 
    // }
    if (!loop_start.empty()) {
        int startConditionIndex = loop_start.top();
        poliz.push_back ( Lex ( POLIZ_LABEL, startConditionIndex ) );
        poliz.push_back(POLIZ_GO);
       
    }
    else throw "continue outside of loop";
}

void Parser::dec ( type_of_lex type ) {
    int i;
    while ( !st_int.empty () ) {
        from_st (st_int, i);
        if ( TID[i].get_declare () ) 
            throw "twice";
        else {
            TID[i].put_declare ();
            TID[i].put_type ( type );
        }
    }
}
 
void Parser::check_id (int value) {

    if (value==-1) value=c_val;

    if ( TID[value].get_declare() ){
        st_lex.push ( TID[value].get_type () );
        //std::cout<< TID[value].get_type ();
        }
    else {
        throw "not declared";}
}
 
void Parser::check_op () {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL, s=LEX_STRING;
 
    from_st ( st_lex, t2 );
    from_st ( st_lex, op );
    from_st ( st_lex, t1 );

    //std::cout<< t1<<op<<t2<<std::endl;
    if (op == LEX_EQ  || op == LEX_LSS || op == LEX_GTR || op == LEX_NEQ)
        { if (t1 == LEX_STRING) {t = LEX_STRING;} 
        }

    if (op == LEX_PLUS ){
        if (t1 == LEX_STRING) {t = LEX_STRING; r = LEX_STRING;} 
        else  r = LEX_INT;
    }

    if (op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH )
        r = LEX_INT;

    if ( op == LEX_OR || op == LEX_AND )
        t = LEX_BOOL;

    if ( t1 == t2  &&  t1 == t ) 
        st_lex.push (r);
    else
        throw "wrong types are in  operation";
    poliz.push_back (Lex (op) );
}
 
void Parser::check_unary () {
    type_of_lex t1,  op, t = LEX_INT;
 
    from_st ( st_lex, t1 );
    from_st ( st_lex,  op );
    //std::cout<< std::endl<<t1<<op<<std::endl;
    if ( t1 == t ) 
        st_lex.push (t);
    else
        throw "wrong types are in unary operation";
   // poliz.push_back (Lex (op) );
}

void Parser::check_not () {
    if (st_lex.top() != LEX_BOOL)
        throw "wrong type is in not";
     else  
        poliz.push_back ( Lex (LEX_NOT) );
}
 
void Parser::eq_type () {
    type_of_lex t;
    from_st ( st_lex, t );
    if ( t != st_lex.top () )
        throw "wrong types are in =";
    //st_lex.pop();
}
 
void Parser::eq_bool () {
    if ( st_lex.top () != LEX_BOOL )
        throw "expression is not boolean";
    st_lex.pop ();
  }
 
void Parser::check_id_in_read () {
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}



///////////////////////////////////////////////////////////////
class Executer {
public:
    void execute ( std::vector<Lex> & poliz );
};
 
void Executer::execute ( std::vector<Lex> & poliz ) {
    Lex pc_el;
    std::stack<Lex> args;
    std::stack<Lex> tempStack; // Временный стек для аргументов write
    std::stack<Lex> tempStack_reverse;   //REVERSE STACK нужен для аргументов write
    bool isWriting = false; // Флаг, указывающий, что мы в режиме записи аргументов write
    Lex i, j;
    bool resultf;
    int  index = 0, size = poliz.size(),var_index,y;
    std::string value_string;
    while ( index < size ) {
        pc_el = poliz [ index ];
        switch ( pc_el.get_type () ) {
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case LEX_STR:
                if (isWriting) {
                    tempStack.push(pc_el); // Сохраняем в tempStack
                } else {
                    args.push(pc_el);
                }
                break;
            case POLIZ_LABEL:
                args.push(pc_el);
                break;
            case LEX_ID:
                if (isWriting){
                
                 y = pc_el.get_value ();
                if ( TID[y].get_assign () ) {
                      if (TID[pc_el.get_value()].get_type() == LEX_INT) {
                        pc_el = Lex(LEX_NUM, TID[pc_el.get_value()].get_value());
                    } else if (TID[pc_el.get_value()].get_type() == LEX_STRING) {
                        pc_el = Lex(LEX_STR, TID[pc_el.get_value()].get_str_value());
                    }
                    else if (TID[pc_el.get_value()].get_type() == LEX_BOOL) {
                        pc_el = Lex(LEX_NUM, TID[pc_el.get_value()].get_value());
                    }
                    tempStack.push( pc_el);
                }
                else
                  throw "POLIZ: indefinite identifier";}
                else {
                 y = pc_el.get_value ();

                if ( TID[y].get_assign () ) {
                    if (TID[pc_el.get_value()].get_type() == LEX_INT) {
                        pc_el = Lex(LEX_NUM, TID[pc_el.get_value()].get_value());
                    } else if (TID[pc_el.get_value()].get_type() == LEX_STRING) {
                        pc_el = Lex(LEX_STR, TID[pc_el.get_value()].get_str_value());
                    }
                    else if (TID[pc_el.get_value()].get_type() == LEX_BOOL) {
                        pc_el = Lex(LEX_NUM, TID[pc_el.get_value()].get_value());
                    }
                    args.push ( pc_el);
                }
                else
                  throw "POLIZ: indefinite identifier";   
                }
                break;
            case LEX_NOT:
                if (!isWriting){
                from_st ( args, i );
                args.push( Lex(LEX_BOOL, !i.get_value()));
                break;}

                else {
                from_st ( tempStack, i );
                tempStack.push( Lex(LEX_BOOL, !i.get_value() ));
                break; 
                }
 
            case LEX_OR:
                if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                args.push( Lex(LEX_BOOL,i.get_value()||j.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                tempStack.push( Lex(LEX_BOOL,i.get_value()||j.get_value()));
                }

                break;
 
            case LEX_AND:
                if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                args.push( Lex(LEX_BOOL, i.get_value()&&j.get_value()));
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                tempStack.push( Lex(LEX_BOOL,i.get_value()&&j.get_value()));
                }

                break;
 
            case POLIZ_GO:
                from_st ( args, i );
                index = i.get_value() - 1;
                break;
 
            case POLIZ_FGO:
                from_st ( args, i );
                from_st ( args, j );
                if ( !(j.get_value()) ) index = i.get_value() - 1;
                break;

            case LEX_WRITE_START:
                isWriting = true; // Включаем режим записи
                break;
                
            case LEX_WRITE:
                // Выводим все значения из временного стека (перед этим реверснув временный стек)
                while (!tempStack.empty()) {
                    tempStack_reverse.push(tempStack.top()); 
                    tempStack.pop();
                }

                while (!tempStack_reverse.empty()) {
                    if ((tempStack_reverse.top().get_type()) == LEX_STR) std::cout << tempStack_reverse.top().get_str() << " ";
                    else std::cout << tempStack_reverse.top().get_value() << " ";
                    tempStack_reverse.pop();
                }
                std::cout << std::endl;
                isWriting = false; // Выключаем режим записи
                break;
 
            case LEX_READ:
                int k;
                from_st ( args, i );
                var_index = i.get_value();
                if ( TID[var_index].get_type () == LEX_INT ) {
                    std::cout << "Input int value for " << TID[var_index].get_name () << ": ";
                    std::cin >> k;
                    TID[var_index].put_value (k);
                    TID[var_index].put_assign ();
                }
                else if (TID[var_index].get_type() == LEX_STRING) {
                        std::cout << "Input string value for " << TID[var_index].get_name() << ": ";

                        std::cin >> value_string;
                        TID[var_index].put_str_value(value_string);
                        TID[var_index].put_assign();
                }
                else {
                    std::string j;
                    while (1) {
                        std::cout << "Input boolean value (true or false) for " << TID[var_index].get_name() << ": ";
                        std::cin >> j;
                        if ( j != "true" && j != "false" ) {
                            std::cout << "Error in input:true/false" << std::endl;
                            continue;
                        }
                        k = ( j == "true" ) ? 1 : 0;
                        break;
                    }
                    TID[var_index].put_value (k);
                    TID[var_index].put_assign ();
                }
                break;
            
            case LEX_PLUS:
                if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                 if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    int result = i.get_value() + j.get_value();
                    args.push(Lex(LEX_NUM, result));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                    std::string result = j.get_str() + i.get_str();
                    args.push(Lex(LEX_STR, result));
                }
               }

                else {
                    
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                       if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    int result = i.get_value() + j.get_value();
                     tempStack.push(Lex(LEX_NUM, result));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                    std::string result = j.get_str() + i.get_str();
                     tempStack.push(Lex(LEX_STR, result));
                }
               
                }

                break;

            case LEX_UNARY_PLUS:
                if (!isWriting){
                from_st ( args, i );
                args.push( Lex(LEX_NUM, +i.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                tempStack.push( Lex(LEX_NUM,+i.get_value()));
                }

                break;

            case LEX_UNARY_MINUS:
                 if (!isWriting){
                from_st ( args, i );
                args.push( Lex(LEX_NUM, -i.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                tempStack.push( Lex(LEX_NUM,-i.get_value()));
                }

                break;

            case LEX_TIMES:
                  if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                args.push( Lex(LEX_NUM, i.get_value()*j.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                tempStack.push( Lex(LEX_NUM,i.get_value()*j.get_value() ));
                }

                break;
 
            case LEX_MINUS:
                  if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                args.push( Lex(LEX_NUM, j.get_value() -i.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                tempStack.push( Lex(LEX_NUM,j.get_value()-i.get_value() ));
                }

                break;
            case LEX_SLASH:
                if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                if (i.get_value()) {
                    args.push (  Lex(LEX_NUM,j.get_value()/i.get_value() ));
                    
                }
                else
                    throw "POLIZ:divide by zero";
                }
                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                if (i.get_value()) {
                    tempStack.push (  Lex(LEX_NUM,j.get_value()/i.get_value() ));
                    
                }
                else
                    throw "POLIZ:divide by zero";
                }
                break;
            case LEX_EQ:
                
                 if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                 if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    resultf = i.get_value() == j.get_value();
                    args.push(Lex(LEX_BOOL, resultf));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                    resultf = i.get_str() == j.get_str();
                    args.push(Lex(LEX_BOOL, resultf));
                }
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                       if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    resultf = i.get_value() == j.get_value();
                     tempStack.push(Lex(LEX_NUM, resultf));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                   resultf = i.get_str() ==j.get_str();
                     tempStack.push(Lex(LEX_STR, resultf));
                } 
                }
                break;
            case LEX_LSS:
               
                 if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                 if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    resultf = i.get_value() > j.get_value();
                    args.push(Lex(LEX_BOOL, resultf));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                    resultf = i.get_str() >j.get_str();
                    args.push(Lex(LEX_BOOL, resultf));
                }
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                       if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    resultf = i.get_value() > j.get_value();
                     tempStack.push(Lex(LEX_NUM, resultf));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                   resultf = i.get_str() >j.get_str();
                     tempStack.push(Lex(LEX_STR, resultf));
                } 
                }
                break;
 
            case LEX_GTR:
                
                 if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                 if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    resultf = i.get_value() <j.get_value();
                    args.push(Lex(LEX_BOOL, resultf));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                    resultf = i.get_str() <j.get_str();
                    args.push(Lex(LEX_BOOL, resultf));
                }
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                       if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    resultf = i.get_value() < j.get_value();
                     tempStack.push(Lex(LEX_NUM, resultf));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                   resultf = i.get_str() <j.get_str();
                     tempStack.push(Lex(LEX_STR, resultf));
                } 
                }
                break;
            case LEX_LEQ:
                  if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                args.push( Lex(LEX_BOOL, j.get_value() <=i.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                tempStack.push( Lex(LEX_BOOL,j.get_value()<=i.get_value() ));
                }

                break;
 
            case LEX_GEQ:
                 if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                args.push( Lex(LEX_BOOL, j.get_value() >=i.get_value() ));
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                tempStack.push( Lex(LEX_BOOL,j.get_value()>=i.get_value() ));
                }

                break;
 
            case LEX_NEQ:
                bool result;
                 if (!isWriting){
                from_st ( args, i );
                from_st ( args, j );
                 if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    result = i.get_value() != j.get_value();
                    args.push(Lex(LEX_BOOL, result));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                    result = i.get_str() != j.get_str();
                    args.push(Lex(LEX_BOOL, result));
                }
               }

                else {
                from_st ( tempStack, i );
                from_st ( tempStack, j);
                       if ((i.get_type() == LEX_NUM && j.get_type() == LEX_NUM)) {
                  
                    result = i.get_value() != j.get_value();
                     tempStack.push(Lex(LEX_NUM, result));
                } else if ((i.get_type() == LEX_STR && j.get_type() == LEX_STR)) {
                    
                   result = i.get_str() !=j.get_str();
                     tempStack.push(Lex(LEX_STR, result));
                } 
                }
                break;
 
            case LEX_ASSIGN:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i );
                if ((i.get_type())== LEX_STR){
                    TID[j.get_value()].put_str_value (i.get_str());
                    }
                else {
                    TID[j.get_value()].put_value (i.get_value());
                }
                TID[j.get_value()].put_assign (); 
                break;
 
            default:
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };
    //end of while
    // int m=0;
    // for (int i=0;i<args.size();) {
    //     from_st(args, m);
    //     std::cout << m<< "    "<<std::endl;
    //     i++;}
    std::cout<< std::endl;
    std::cout << "Finish of executing!!!" <<std::endl;
}
 
class Interpretator {
    Parser   pars;
    Executer E;
public:
    Interpretator ( const char* program ): pars (program) {}
    void     interpretation ();
};
 
void Interpretator::interpretation () {
    pars.analyze ();
    std::cout<< std::endl;
    std::cout<<"EXECUTING PROGRAM:"<<std::endl;
    std::cout<< std::endl;
    E.execute ( pars.poliz );
}


int main () {

    try {  
        
        Interpretator I ( "tests1.txt" );
        I.interpretation ();
        return 0;
      
    }
    catch ( char c ) {
        std::cout << "unexpected symbol " << c << std::endl;
        return 1;
    }
    catch (  Lex &l ) {
        std::cout << "unexpected lexeme" << l<< std::endl;
        return 1;
    }
    catch ( const char *source ) {
        std::cout << source << std::endl;
        return 1;
    }
    catch (...) {
    std::cerr << std::endl << "Nooooo(( Your program is not correct ;((" << std::endl;
    return 1;
}
}
 