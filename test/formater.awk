BEGIN{
    FS=";"
    OFS=","
}
{
    

    #class 
    if($1 == ""){
        next
    }
    gsub(/p/, "0", $1)
    gsub(/e/, "1", $1)

    #cap-shape 
    if($3 == ""){
        $3 = "0"
    }
    gsub(/b/, "1", $3)
    gsub(/c/, "2", $3)
    gsub(/x/, "3", $3)
    gsub(/f/, "4", $3)
    gsub(/s/, "5", $3)
    gsub(/p/, "6", $3)
    gsub(/o/, "7", $3)

    #cap-surface
    if($4 == ""){
        $4 = "0"
    }
    gsub(/i/, "1", $4)
    gsub(/g/, "2", $4)
    gsub(/y/, "3", $4)
    gsub(/s/, "4", $4)
    gsub(/h/, "5", $4)
    gsub(/l/, "6", $4)
    gsub(/k/, "7", $4)
    gsub(/t/, "8", $4)
    gsub(/w/, "9", $4)
    gsub(/e/, "10", $4)

    #cap-color
    if($5 == ""){
        $5 = "0"
    }
    gsub(/n/, "1", $5)
    gsub(/b/, "2", $5)
    gsub(/g/, "3", $5)
    gsub(/r/, "4", $5)
    gsub(/p/, "5", $5)
    gsub(/u/, "6", $5)
    gsub(/e/, "7", $5)
    gsub(/w/, "8", $5)
    gsub(/y/, "9", $5)
    gsub(/l/, "10", $5)
    gsub(/o/, "11", $5)
    gsub(/k/, "12", $5)

    #TODO : CONTINUE

    gsub(/;/, ",") 

    print
}