signature PCOMB =
sig
  datatype parserError = EOF
                       | Unexpected of string;

  datatype 'a presult = PResult of 'a * string
                      | PError of (parserError * string);

  datatype 'a parser = Parser of (string -> 'a presult);

  val runParser : 'a parser -> (string -> 'a presult)
  val <$> : ('a -> 'b) * 'a parser -> 'b parser
  val <*> : ('a -> 'b) parser * 'a parser -> 'b parser
  val pure : 'a -> 'a parser
  val >>= : 'a parser * ('a -> 'b parser) -> 'b parser
  val >> : 'a parser * 'b parser -> 'b parser
  val <|> : 'a parser * 'a parser -> 'a parser
  val many : 'a parser -> 'a list parser
  val some : 'a parser -> 'a list parser
  val item : char parser
end
