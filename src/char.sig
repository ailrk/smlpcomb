signature CHAR =
sig
  include PCOMB
  val satisfy : (char -> bool) -> char parser
  val ch : char -> char parser
  (* val digit : int parser *)
  val space : char parser
  (* val str : string -> string parser *)
end
