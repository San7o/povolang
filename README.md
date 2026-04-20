# povolang

Embeddable programming languge, will become the default shell for
[povOS](https://github.com/San7o/povos).

Of course, this is all hand-written. FWIW, the order in which I
developed things is the following (and you should probably stick to it
when developing a programming language):

- define the grammar in BNF form
- define the AST
- write the tokenizer
- write the parser
- write the codegen

We are using a recursive descent parser with one lookahead token
(LL(1) grammar). The grammar is defined in the GRAMMAR file.
