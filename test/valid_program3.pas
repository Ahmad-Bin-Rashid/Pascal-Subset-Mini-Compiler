program ValidProg3;

const
  THIRTY = 30;
  TRUEVAL = true;

var
  userInput, calc : integer;
  flag : boolean;

begin
  write('Enter integer value: ');
  readln(userInput);

  if userInput < THIRTY then
  begin
    calc := userInput * 2;
    flag := TRUEVAL;
  end
  else
  begin
    calc := userInput - THIRTY;
    flag := false;
  end;

  writeln('Calculated result: ', calc);
  writeln('Flag condition: ', flag);
end.
