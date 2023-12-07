let desmosstr = "";
let cstr = "";

for(let i = 0; i < 100; i++) {
    let x = Math.floor( 100 * Math.random() ** 3 * 100 )/100;
    let y = Math.floor( 100 * (Math.random()) ** 3 * 100) / 100;
    cstr += `{${x}, ${y}}, `
    desmosstr += `(${x}, ${y}),`
}
console.log(cstr);
console.log(desmosstr);