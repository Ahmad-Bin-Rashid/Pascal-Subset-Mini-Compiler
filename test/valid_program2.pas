program ValidProg2;

var
  arr : array[1..10] of integer;
  i, sum : integer;

begin
  i := 1;
  while i <= 10 do
  begin
    arr[i] := i * 5;
    i := i + 1;
  end;

  sum := 0;
  for i := 1 to 10 do
  begin
    sum := sum + arr[i];
  end;

  writeln('Calculated Array Sum: ', sum);
end.
