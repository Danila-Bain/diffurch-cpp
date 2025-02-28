(* classical runge kutta of 4th order *)
a = {
{0,0,0,0},
{1/2,0,0,0},
{0, 1/2, 0, 0},
{0,0,1,0}
};
c = {0,1/2,1/2,1};
b = {1/6,2/6,2/6,1/6};
btilda = {t - 3 t^2/2 + 2 t^3/3, t^2 - 2 t^3/3, t^2 - 2 t^3/3, -t^2/2 + 2 t^3/3};

digits = 50;

FormatList[l_] := NumberForm[N[#, digits], {Infinity,digits}]& /@ (l //. {el___, 0} -> {el} /. 0->0.) // ToString//InputForm;

FormatList @ a

FormatList @ b


FormatList @ bb

FormatList @ c

Length[a]

PolynomialListCoefficients[bs_] := Table[Coefficient[bs[[i]], t, j - 1], {i, 1, Length[bs]}, {j, 1, Max[Length @ #& /@ CoefficientList[bs, t]]}];

Max[Length @ #& /@ CoefficientList[btilda, t]]

FormatList@PolynomialListCoefficients[btilda]
