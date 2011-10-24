import sys
sys.path.append('..')

import wikipedia, config, query

def check_namespaces(site):
    try:
        if not site.apipath():
            wikipedia.output(u'Warning! %s has no apipath() defined!' % site)
            return
    except NotImplementedError:
#     TODO: If use Special:Export to get XML file and parse details in <namespaces></namespaces>,
#     we can get the namespace names without API.
        wikipedia.output(u'Warning! %s is not support API!' % site)
        return
    result = []
    for namespace in site.siteinfo('namespaces').itervalues():
        try:
            defined_namespace = site.namespace(namespace['id'])
        except KeyError:
            wikipedia.output(u'Warning! %s has no _default for namespace %s' % \
                (site, namespace['id']))
            defined_namespace = None

        if defined_namespace != namespace['*'] and namespace['*']:
            result.append((namespace['id'], namespace['*'], defined_namespace))
    return result

def check_family(family):
    wikipedia.output(u'Checking namespaces for %s' % family.name)
    result = {}
    for lang in family.langs:
        if not lang in family.obsolete:
            site = wikipedia.getSite(lang, family)
            wikipedia.output(u'Checking %s' % site)
            namespaces = check_namespaces(site)
            if namespaces:
                for id, name, defined_namespace in namespaces:
                    wikipedia.output(u'Namespace %s for %s is %s, %s is defined in family file.' % \
                        (id, site, name, defined_namespace))
                result[lang] = namespaces
    return result

if __name__ == '__main__':
    try:
        wikipedia.handleArgs()
        family = wikipedia.Family(wikipedia.default_family)
        result = check_family(family)
        wikipedia.output(u'Writing raw Python dictionary to stdout.')
        wikipedia.output(u'Format is: (namespace_id, namespace_name, predefined_namespace)')
        print result
    finally:
        wikipedia.stopme()
