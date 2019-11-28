// Copyright 2019 Association for the promotion of open - source insurance software and for the establishment of open interface standards in the insurance industry
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

grammar RatingEngine;

parse
 : block EOF
 ;

block
 : ifStatement | (Return expression) | errorFunctionCall
 ;

errorFunctionCall
 : Error '(' expression ')'
 ;

functionCall
 : Max '(' expression ',' expression ')'   #maxFunctionCall
 | Min '(' expression ',' expression ')'   #minFunctionCall
 | Rnd '(' expression ',' expression ')'   #roundFunctionCall
 | Ceil '(' expression ')'                 #ceilFunctionCall
 | Floor '(' expression ')'                #floorFunctionCall
 | Exp '(' expression ')'                  #expFunctionCall
 | Day '(' expression ')'                  #dayFunctionCall
 | Month '(' expression ')'                #monthFunctionCall
 | Year '(' expression ')'                 #yearFunctionCall
 | Substr '(' expression ',' expression ',' expression')'   #substrFunctionCall
 | AddDays '(' expression ',' expression ')'   #addDaysFunctionCall
 | GetDiffDays '(' expression ',' expression ')'   #getDiffDaysFunctionCall
 | PaddedString '(' expression ',' expression ')'   #paddedStringFunctionCall
 | DifferenceInMonths '(' expression ',' expression ')'   #differenceInMonthsFunctionCall
 | Identifier '(' exprList? ')'            #identifierFunctionCall
 ;

ifStatement
 : ifStat elseIfStat* elseStat? End
 ;

ifStat
 : If '(' expression ')' Then block
 ;

elseIfStat
 : Else If '(' expression ')' Then block
 ;

elseStat
 : Else block
 ;

exprList
 : expression (',' expression)*
 ;

expression
 : '-' expression                           #unaryMinusExpression
 | '!' expression                           #notExpression
 | expression '^' expression                #powerExpression
 | expression '*' expression                #multiplyExpression
 | expression '/' expression                #divideExpression
 | expression '+' expression                #addExpression
 | expression '-' expression                #subtractExpression
 | expression '>=' expression               #gtEqExpression
 | expression '<=' expression               #ltEqExpression
 | expression '>' expression                #gtExpression
 | expression '<' expression                #ltExpression
 | expression '=' expression                #eqExpression
 | expression '<>' expression               #notEqExpression
 | expression And expression                #andExpression
 | expression Or expression                 #orExpression
 | expression Mod expression                #moduloExpression
 | Number                                   #numberExpression
 | Bool                                     #boolExpression
 | functionCall                             #functionCallExpression
 | Identifier                               #identifierExpression
 | String                                   #stringExpression
 | '(' expression ')'                       #expressionExpression
 ;


If       :  I F;
Then     :  T H E N;
Else     :  E L S E;
Return   :  R E T U R N;
End      :  E N D;
Or       :  O R;
And      :  A N D;
Max      :  M A X;
Min      :  M I N;
Rnd      :  R N D;
Ceil     :  C E I L;
Floor    :  F L O O R;
Exp      :  E X P;
Year     :  Y E A R;
Month    :  M O N T H;
Day      :  D A Y;
Substr   :  S U B S T R;
Error    :  E R R O R;
Mod      :  M O D;
AddDays  :  A D D D A Y S;
GetDiffDays : G E T D I F F D A Y S;
PaddedString : P A D D E D S T R I N G;
DifferenceInMonths : D I F F E R E N C E I N M O N T H S;

A : 'A'|'a';
B : 'B'|'b';
C : 'C'|'c';
D : 'D'|'d';
E : 'E'|'e';
F : 'F'|'f';
G : 'G'|'g';
H : 'H'|'h';
I : 'I'|'i'; 
L : 'L'|'l';
M : 'M'|'m';
N : 'N'|'n';
O : 'O'|'o';
P : 'P'|'p';
R : 'R'|'r';
S : 'S'|'s';
T : 'T'|'t';
U : 'U'|'u';
X : 'X'|'x';
Y : 'Y'|'y';

Equals   : '=';
NEquals  : '<>';
GTEquals : '>=';
LTEquals : '<=';
Pow      : '^';
Excl     : '!';
GT       : '>';
LT       : '<';
Add      : '+';
Subtract : '-';
Multiply : '*';
Divide   : '/';
Modulus  : '%';

Bool
 : 'true' 
 | 'false'
 ;

Number
 : Int ('.' Digit*)?
 ;

Identifier
 : [a-zA-Z_] [a-zA-Z_0-9]*
 ;

String
 : ['] (~['\r\n] | '\\\\' | '\\\'')* [']
 ;

Space
 : [ \t\r\n\u000C] -> skip
 ;

Line_Comment
 : '//' .*? '\r'? '\n' -> skip
 ;

Comment
 : '/*' .*? '*/' -> skip
 ;

fragment Int
 : [1-9] Digit*
 | '0'
 ;
  
fragment Digit 
 : [0-9]
 ;
 
 // handle characters which failed to match any other token
ErrorCharacter : . ;
