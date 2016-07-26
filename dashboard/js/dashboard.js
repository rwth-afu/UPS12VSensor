var g11, g12, g21, g22;
var defaults1 = {
	decimals: 2,
	value: 0.0,
	min: 0.0,
	max: 15.0,
	title: "Spannung"
};
var defaults2 = {
	decimals: 2,
	value: 0.0,
	min: 0.0,
	max: 5.0,
	title: "Strom"
};

$(function() {
	g11 = new JustGage({ id: "11", defaults: defaults1 });
	g12 = new JustGage({ id: "12", defaults: defaults2 });
	g21 = new JustGage({ id: "21", defaults: defaults1 });
	g22 = new JustGage({ id: "22", defaults: defaults2 });

	loadValues();
	setInterval(loadValues, 1000 * 1);
});

function loadValues() {
	$.get("./readvalues.php", function(data) {
		var values = data.split(";");

		g11.refresh(values[0]);
		g12.refresh(values[1]);
		g21.refresh(values[2]);
		g22.refresh(values[3]);
	});
}
