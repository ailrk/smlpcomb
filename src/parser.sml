structure Parser :> PCOMB =
struct
  open Util;
  datatype parserError = EOF
                       | Unexpected of string;

  datatype 'a presult = PResult of 'a * string
                      | PError of (parserError * string);

  datatype 'a parser = Parser of (string -> 'a presult);

  fun runParser (Parser p) = p;

  infix 2 <$>;
  fun f <$> (Parser p) = Parser (fn s =>
    case p s of
         PError e => PError e
       | PResult (a, s') => PResult (f a, s'));

  fun pure a = Parser (fn s => PResult (a, s));

  infix 2 <*>;
  fun (Parser fm) <*> (Parser m) = Parser (fn s =>
    case fm s of
         PError e => PError e
       | PResult (f, s') =>
           case m s' of
                PError e => PError e
              | PResult (v, s'') => PResult (f v, s''));

  infixr 1 >>=;
  fun (Parser ma) >>= f = Parser (fn s =>
    case ma s of
         PError e => PError e
       | PResult (a, s') =>
           case runParser (f a) s' of
                PError e => PError e
              | PResult (b, s'') => PResult (b, s''));

  infixr 1 >>;
  fun ma >> mb = ma >>= (fn _ => mb);

  infix 1 <|>;
  fun (Parser a) <|> (Parser b) = Parser (fn s =>
    case a s of
         PError _ => b s
       | PResult (v, s') => PResult (v, s'))

  (* without lazyniess you can't have this *)
  (* fun many v = (cons <$> v <*> many v) <|> pure []; *)
  (* fun some v = cons <$> v <*> many v; *)

  fun many v = Parser (fn s =>
      case runParser v s of
           PError (e, s') => PResult ([], s')
         | PResult (a, s') =>
             let
               val cons = (pure o curry (op ::)) a;
               val p = cons <*> many v;
             in runParser p s' end);

  fun some v = (curry (op ::)) <$> v <*> many v;

  val item = Parser (fn s =>
    if String.size s = 0
    then PError (EOF, s)
    else
      let
        val head = String.sub (s, 0);
        val tail = String.extract (s, 1, NONE);
      in PResult (head, tail) end);

 end
