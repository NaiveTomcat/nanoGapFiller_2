with open('Output.sitegraph') as f:
    with open('Output.csv', 'w') as f2:
        for l in f:
            if 'of' in l:
                f2.write('{},{},{}\n'.format(l.split(' ')[6],l.split(' ')[2], l.split(' ')[5]))