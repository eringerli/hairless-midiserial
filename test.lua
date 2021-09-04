
function onMessageLeft (x)
    print("onMessageLeft:")
    for k=1,#x do
        v = x[k]
        print("\t", k, v)
    end
    print()
    sendMessageRight(x)
end

function onMessageRight (x)
    print("onMessageLeft:")
    for k=1,#x do
        v = x[k]
        print("\t", k, v)
    end
    print()
    sendMessageLeft(x)
end
