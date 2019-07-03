open GraphqlHooksTypes;

module Client = GraphqlHooksClient;

module Memcache = GraphqlHooksMemcache;

module Context = GraphqlHooksContext;
module Provider = GraphqlHooksContext.Provider;

module CreateQuery = (Config: Config) => Query.Make(Config);

module CreateMutation = (Config: Config) => Mutation.Make(Config);