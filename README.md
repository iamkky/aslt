# A Simple Language Toolset

The purpose of ASLT is to provide a very basic tool to generate parsers and lex analyzers.
And, of course, a little bit of fun to me :-)

## Philosophy

The approach was to try a minimalist solution, that could be useful without being complex.
It is important to warn that the "minimalist" term used does not referer to any sort of super-uber-compact crypto-esoteric code.
It's not about the number of characters in code.

The minimalist in this case is about to control the scope and requirements in order to be possible too provide a small, flexible and very generic solution to a sub set of what most "serious" language tools out there proposes.

## NRLEX - Non Regex Lex Generator

nrlex is a lex analyzer generator based in a very simples state machine.
The code generated is linear and very simple.
Understanding the generated code is very easy, as is hacking into it.
And with a little of practice nrlex can be tricked to do a really nice work.

In order to keep it "minimalist", I decided to not bother about full regex support. Most of my needs are very basic and hopefully can be accomplished without regex.

In fact there is a lot of useful tasks that a non regex tokenizer can accomplish. Numbers, keywords, operators, identifiers, all can be easily specified to nrlex.

Tokenization of strings requires a little bit more of work. But for this kind of situation nrlex can be easily supported by the way of a specific detection function.

I didn't want a generator that puts out unreadable code.

## RDPPGEN - Recursive Descendent Predictive Parser Generator

rdppgen is rooted on the same principles of the nrlex. And again I really don't need a general parser generator for most of my needs.
As a recursive descendent parser has a wide range of applications, having a generator that can put out a nice readable code sounded like a nice idea.

That was the way rdppgen come out to life. The purpose was to have a generator putting out a code that could have being handwritten!

Without lookahead, and backtracking capability I expected to have a very limited parser generator. But after trying to use it, and adding some hooks to work around those limitations I ended up with a pretty capable parser generator.

## Capability

Even though this toolset is much more in the domain of "toy" tools, they showed a really nice usability and kind of a fun experiment.

#### C parser using ASLT

With a few tweaks to syntax definition, and using flexibility of rdppgen to insert custom code into the generated parser I could even manage to build C parser using it.

The parser successfully parsed some C89 code.

The lex can be found at [c_lex.nrlex](https://github.com/iamkky/cparser/blob/master/c_lex.nrlex),
the parser can be found at [c_parser.rdpp](https://github.com/iamkky/cparser/blob/master/c_parser.rdpp),
and the complete project is at [C Parser with ASLT](https://github.com/iamkky/cparser)





