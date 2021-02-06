structure CharParser :> CHAR =
struct
  open Parser;
  open Util;
  fun satisfy pred = Parser (fn s =>
    case runParser item s of
         PError e => PError e
       | PResult (v, s') =>
           if pred v
           then PResult (v, s')
           else PError (Unexpected ("unexpted character " ^ Char.toString v) ,s'))

  fun ch c = satisfy (fn c' => c = c');

  (* val digit = char2int <$> satisfy Char.isDigit; *)

  val space = satisfy Char.isSpace;

  (* fun str s = *)
  (*   let *)
  (*     val slist = String.explode s; *)
  (*     fun str' (s' : char list) = *)
  (*       case s' of *)
  (*            [] => pure "" *)
  (*          | (x::xs) => ch x >> str' xs >> pure s *)
  (*   in str' slist end *)
end
