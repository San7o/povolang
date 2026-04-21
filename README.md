# povolang

Embeddable programming languge, will become the default shell for
[povOS](https://github.com/San7o/povos).

Of course, this is all hand-written C99. FWIW, the order in which I
developed things is the following (and you should probably stick to it
when developing a programming language):

- define the grammar in BNF form
- define the AST
- write the tokenizer
- write the parser
- write the codegen

The most important file is "lang.h". The grammar is defined in the
GRAMMAR file, it resembles javascript. The grammar is LL(1) so I can
use a neat recursive descent parser with one lookahead token. It uses
a token stream which computes the tokens ahead of time.

Soon I'll generate LLVM IR, then I will implement a proper backend.
