structure Util =
struct
  val curry = fn f => fn x => fn y => f (x, y);
  fun char2int (n: char) : int = Char.ord n - Char.ord #"0";
end
