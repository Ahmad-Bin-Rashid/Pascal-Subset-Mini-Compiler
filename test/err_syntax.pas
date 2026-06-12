program ErrSyntax;

var
  x, y : integer;

begin
  x := 10 
  y := 20;
  if x > y then
    writeln('Greater')
  else
    writeln('Lesser');
  
  writeln(x y); 
end.
