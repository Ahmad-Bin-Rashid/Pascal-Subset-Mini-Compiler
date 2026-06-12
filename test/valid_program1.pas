program ValidProg1;

const
  MAX = 100;
  PI = 3.1415;
  BANNER = 'Compiler Project Demo';

var
  x, y, result : integer;
  flag : boolean;

procedure greet(times : integer);
var
  i : integer;
begin
  for i := 1 to times do
  begin
    writeln(BANNER);
  end;
end;

function computeSquare(val : integer) : integer;
begin
  computeSquare := val * val;
end;

function checkGreater(a : integer; b : integer) : boolean;
begin
  if a > b then
    checkGreater := true
  else
    checkGreater := false;
end;

begin
  x := 10;
  y := 20;
  greet(3);
  
  if checkGreater(y, x) then
  begin
    result := computeSquare(y);
    writeln('Result is ', result);
  end
  else
  begin
    result := computeSquare(x);
    writeln('Result is ', result);
  end;
end.
