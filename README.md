# lang

Embeddable programming languge, will become the default shell for
[povOS](https://github.com/San7o/povos).

Of course, this is all hand-written. The order in which I developed
things is the following:

- define the grammar in BNF form
- define the AST
- write the tokenizer
- write the parser
- write the codegen

We are using a recursive descent parser with one lookahead token
(LL(1) grammar).
