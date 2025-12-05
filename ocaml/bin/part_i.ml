let starting_position = 50
let dial_size = 100

let parse_direction line =
  let magnitude = int_of_string (String.sub line 1 (String.length line - 1)) in
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
        let new_pos = ((pos + delta) mod dial_size + dial_size) mod dial_size in
        let new_count = if new_pos = 0 then count + 1 else count in
        loop new_pos new_count rest
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
