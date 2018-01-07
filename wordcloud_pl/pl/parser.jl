
type Instance
    nb::Int
    rect::Vector{Vector{Float64}}
    Instance() = new(0, [])
end

inst = Instance()

open("../instance.txt") do f

    lines = readlines(f)

    inst.nb = parse(Int, lines[1])

    for i in 2:(inst.nb+1)
        l = split(lines[i], " ")
        push!(inst.rect, [parse(Float64, l[1]), parse(Float64, l[2])])
    end

end
