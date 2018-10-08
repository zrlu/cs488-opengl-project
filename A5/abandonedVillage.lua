rootNode = gr.node('root')

-- The ground must exists and be called 'ground', to demonstrate Perlin noise
ground = gr.mesh('grid', 'ground')
ground:scale(30, 1, 30)
ground:translate(0, 0, 5)
ground:set_material(gr.material({0.85, 0.75, 0.6}, {0.45, 0.45, 0.45}, 10.0, 1.0, "trestraou1", 2.0, { 0.5, 1.5, 60, 0.04 }))
rootNode:add_child(ground)

sphereMesh = gr.mesh('sphere', 'name-of-sphere')
sphereMesh:scale(0.5, 0.5, 0.5)
sphereMesh:rotate('y', 90)
sphereMesh:translate(-1, 3.8, 10)
sphereMesh:set_material(gr.material({0.25, 0.5, 0.75}, {1, 1, 1}, 5, 0.5, nil, 1.0, { 0.5, 0.0, 0.0, 0.0 } ))
rootNode:add_child(sphereMesh)

well = gr.mesh('well', 'name-of-sphere')
well:scale(1, 1, 1)
well:translate(-1, -0.2, 10)
well:set_material(gr.material({0.8, 0.8, 0.8}, {0.25, 0.15, 0.1}, 30, 1, "Brick_floor", 1.0, { 0.02, 0.0, 0.0, 0.0 } ))
rootNode:add_child(well)

cabin_mat = gr.material({1.0, 1.0, 1.0}, {0.05, 0.05, 0.05}, 2.0, 1, "WoodCabin", 1.0, { 0.0, 0.0, 0.0, 0.0 } )

cabin = gr.mesh('cabin', 'cabin')
cabin:scale(0.1, 0.1, 0.1)
cabin:translate(1, -0.1, 0)
cabin:set_material(cabin_mat)
rootNode:add_child(cabin)

cabin2 = gr.mesh('cabin', 'cabin2')
cabin2:rotate('y', 45)
cabin2:scale(0.1, 0.1, 0.1)
cabin2:translate(-10, -0.1, -2)
cabin2:set_material(cabin_mat)
rootNode:add_child(cabin2)

cabin3 = gr.mesh('cabin', 'cabin3')
cabin3:rotate('y', 180)
cabin3:scale(0.1, 0.1, 0.1)
cabin3:translate(10, -0.1, 15)
cabin3:set_material(cabin_mat)
rootNode:add_child(cabin3)

cabin4 = gr.mesh('cabin', 'cabin4')
cabin4:rotate('y', 170)
cabin4:scale(0.1, 0.1, 0.1)
cabin4:translate(0, -0.1, 20)
cabin4:set_material(cabin_mat)
rootNode:add_child(cabin4)

cabin5 = gr.mesh('cabin', 'cabin5')
cabin5:rotate('y', 180)
cabin5:scale(0.1, 0.1, 0.1)
cabin5:translate(-8, -0.1, 15)
cabin5:set_material(cabin_mat)
rootNode:add_child(cabin5)

inn_mat = gr.material({1.0, 1.0, 1.0}, {0.05, 0.05, 0.05}, 2.0, 1, "inn", 1.0, { 0.0, 0.0, 0.0, 0.0 } )

inn = gr.mesh('Medieval_Inn', 'inn')
inn:rotate('y', -45)
inn:scale(1.0, 1.0, 1.0)
inn:translate(10, -0.1, 5)
inn:set_material(inn_mat)
rootNode:add_child(inn)

-- A sun light must exist
-- Sun light direction automatically calculated from time
sunLight = gr.light({0, 0, 0, 0}, {1, 0.98, 0.9}, {1, 0.0, 0.0}, nil, nil)
rootNode:add_light(sunLight)
return rootNode
