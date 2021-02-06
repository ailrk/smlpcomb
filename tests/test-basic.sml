structure TestBasic =
struct
  open CharParser;
  fun test_ch () =
    let
      val p1 = ch #"a";
      val result = runParser p1 "ab"
    in result end

  (* fun test_str () = *)
  (*   let *)
  (*     val p1 = str "string" *)
  (*     val result = runParser p1 "string to parse" *)
  (*   in result end *)

  (* fun test_digit () = runParser digit "1" *)

  fun test_many () =
    let
      val p1 = (many o ch) #"a"
      val result = runParser p1 "aaaab"
   in result end

  fun test_some () =
    let
      val p1 = (some o ch) #"a"
      val result = runParser p1 "aaaab"
      val result1 = runParser p1 "baaab"
    in result1 end
end
