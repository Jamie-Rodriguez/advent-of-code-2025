let starting_position = 50
let dial_size = 100

let parse_direction line =
  (* Normalise to the range [0, dial_size) *)
  let magnitude =
    String.sub line 1 (String.length line - 1)
    |> int_of_string
    |> fun n -> n mod dial_size
  in
  match line.[0] with
  | 'L' -> -magnitude
  | 'R' -> magnitude
  | c -> invalid_arg (Printf.sprintf "Invalid direction: %c" c)

let read_directions filename =
  let ic = open_in filename in
  let rec read_lines acc =
    match input_line ic with
    | line -> read_lines (parse_direction line :: acc)
    | exception End_of_file -> close_in ic; List.rev acc
  in
  read_lines []

let count_zero_positions directions =
  let rec loop pos count = function
    | [] -> count
    | delta :: rest ->
        let linear_pos = pos + delta in
        (* Normalise position into the range [0, dial_size) *)
        let pos =
          if linear_pos < 0 then linear_pos + dial_size
          else if linear_pos >= dial_size then linear_pos - dial_size
          else linear_pos
        in
        loop pos (count + if pos = 0 then 1 else 0) rest
  in
  loop starting_position 0 directions

let () =
  if Array.length Sys.argv < 2 then begin
    Printf.printf "Usage: %s <filename>\n" Sys.argv.(0);
    exit 1
  end;
  let filename = Sys.argv.(1) in
  let directions = read_directions filename in
  let answer = count_zero_positions directions in
  Printf.printf "Password: %d\n" answer
