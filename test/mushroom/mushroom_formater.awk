#format and normalize dataset
#be sure that the final inputs and outputs are in [0,1] range to not saturate the neural network.

#normalize
function n(value, max){
    return value/max;
}


BEGIN{
    FS=";"
    OFS=","
}
{
    

    #class     
    switch($1){
        case "p": $1=0; break;
        case "e": $1=1; break;
        default: next; break;
    }

    #cap-diameter (float)
    max2 = 100;

    #cap-shape     
    switch($3){
        case "b": $3=1; break;
        case "c": $3=2; break;
        case "x": $3=3; break;
        case "f": $3=4; break;
        case "s": $3=5; break;
        case "p": $3=6; break;
        case "o": $3=7; break;
        default: $3=0; break;
    }
    max3=7;

    #cap-surface
    switch($4){
        case "i": $4=1; break;
        case "g": $4=2; break;
        case "y": $4=3; break;
        case "s": $4=4; break;
        case "h": $4=5; break;
        case "l": $4=6; break;
        case "k": $4=7; break;
        case "t": $4=8; break;
        case "w": $4=9; break;
        case "e": $4=10; break;
        default: $4=0; break;
    }
    max4=10;

    #cap-color
    switch($5){
        case "n": $5=1; break;
        case "b": $5=2; break;
        case "g": $5=3; break;
        case "r": $5=4; break;
        case "p": $5=5; break;
        case "u": $5=6; break;
        case "e": $5=7; break;
        case "w": $5=8; break;
        case "y": $5=9; break;
        case "l": $5=10; break;
        case "o": $5=11; break;
        case "k": $5=12; break;
        default: $5=0; break;
    }
    max5=12;
    
    #does-bruise-bleed
    switch($6){
        case "t": $6=1; break;
        case "f": $6=2; break;
        default: $6=0; break;
    }
    max6=2;
    
    #gill-attachment
    switch($7){
        case "a": $7=1; break;
        case "x": $7=2; break;
        case "d": $7=3; break;
        case "e": $7=4; break;
        case "s": $7=5; break;
        case "p": $7=6; break;
        case "f": $7=7; break;
        case "?": $7=8; break;
        default: $7=0; break;
    }
    max7=8;
   
    #gill-spacing
    switch($8){
        case "c": $8=1; break;
        case "d": $8=2; break;
        case "f": $8=3; break;
        default: $8=0; break;
    }
    max8=3;

    #gill-color
    switch($9){
        case "n": $9=1; break;
        case "b": $9=2; break;
        case "g": $9=3; break;
        case "r": $9=4; break;
        case "p": $9=5; break;
        case "u": $9=6; break;
        case "e": $9=7; break;
        case "w": $9=8; break;
        case "y": $9=9; break;
        case "l": $9=10; break;
        case "o": $9=11; break;
        case "k": $9=12; break;
        case "f": $9=13; break;
        default: $9=0; break;
    }
    max9=13;

    #stem-height (float)
    max10=50;

    #stem-width (float)
    max11=150;

    #stem-root
    switch($12){
        case "b": $12=1; break;
        case "s": $12=2; break;
        case "c": $12=3; break;
        case "u": $12=4; break;
        case "e": $12=5; break;
        case "z": $12=6; break;
        case "r": $12=7; break;
        default: $12=0; break;
    }
    max12=7;

    #stem-surface
    switch($13){
        case "i": $13=1; break;
        case "g": $13=2; break;
        case "y": $13=3; break;
        case "s": $13=4; break;
        case "h": $13=5; break;
        case "l": $13=6; break;
        case "k": $13=7; break;
        case "t": $13=8; break;
        case "w": $13=9; break;
        case "e": $13=10; break;
        case "f": $13=11; break;
        default: $13=0; break;
    }
    max13=11;

    #stem-color
    switch($14){
        case "n": $14=1; break;
        case "b": $14=2; break;
        case "g": $14=3; break;
        case "r": $14=4; break;
        case "p": $14=5; break;
        case "u": $14=6; break;
        case "e": $14=7; break;
        case "w": $14=8; break;
        case "y": $14=9; break;
        case "l": $14=10; break;
        case "o": $14=11; break;
        case "k": $14=12; break;
        case "f": $14=13; break;
        default: $14=0; break;
    }
    max14=13;

    #veil-type
    switch($15){
        case "p": $15=1; break;
        case "u": $15=2; break;
        default: $15=0; break;
    }
    max15=2;

    #veil-color
    switch($16){
        case "n": $16=1; break;
        case "b": $16=2; break;
        case "g": $16=3; break;
        case "r": $16=4; break;
        case "p": $16=5; break;
        case "u": $16=6; break;
        case "e": $16=7; break;
        case "w": $16=8; break;
        case "y": $16=9; break;
        case "l": $16=10; break;
        case "o": $16=11; break;
        case "k": $16=12; break;
        case "f": $16=13; break;
        default: $16=0; break;
    }
    max16=13;

    #has-ring
    switch($17){
        case "t": $17=1; break;
        case "f": $17=2; break;
        default: $17=0; break;
    }
    max17=2;

    #ring-type
    switch($18){
        case "c": $18=1; break;
        case "e": $18=2; break;
        case "r": $18=3; break;
        case "g": $18=4; break;
        case "l": $18=5; break;
        case "p": $18=6; break;
        case "s": $18=7; break;
        case "z": $18=8; break;
        case "y": $18=9; break;
        case "m": $18=10; break;
        case "f": $18=11; break;
        case "?": $18=12; break;
        default: $18=0; break;
    }
    max18=12;

    #spore-print-color
    switch($19){
        case "n": $19=1; break;
        case "b": $19=2; break;
        case "g": $19=3; break;
        case "r": $19=4; break;
        case "p": $19=5; break;
        case "u": $19=6; break;
        case "e": $19=7; break;
        case "w": $19=8; break;
        case "y": $19=9; break;
        case "l": $19=10; break;
        case "o": $19=11; break;
        case "k": $19=12; break;
        default: $19=0; break;
    }
    max19=12;

    #habitat
    switch($20){
        case "g": $20=1; break;
        case "l": $20=2; break;
        case "m": $20=3; break;
        case "p": $20=4; break;
        case "h": $20=5; break;
        case "u": $20=6; break;
        case "w": $20=7; break;
        case "d": $20=8; break;
        default: $20=0; break;
    }
    max20=8;

    #season
    switch($21){
        case "s": $21=1; break;
        case "u": $21=2; break;
        case "a": $21=3; break;
        case "w": $21=4; break;
        default: $21=0; break;
    }
    max21=4;

    gsub(/;/, ",") 

    printf "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", 
        $1, n($2,max2), n($3,max3), n($4,max4), n($5,max5),
        n($6,max6), n($7,max7), n($8,max8), n($9,max9), n($10,max10),
        n($11,max11),n($12,max12),n($13,max13),n($14,max14),n($15,max15),
        n($16,max16),n($17,max17),n($18,max18),n($19,max19),n($20,max20),
        n($21,max21);
}